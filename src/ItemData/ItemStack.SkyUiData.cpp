#include "ItemStack.h"
#include "ItemDefines.h"

namespace I4Data::Items {
	void ItemStack::SkyUiProcessEntry() const {
		PROFILE_SCOPE;

		const auto player = RE::PlayerCharacter::GetSingleton();
		const auto value = _data.value.value;
		using std::max;
		const auto weight = max(0.0f, _data.weight.value);

		_data.baseId = _foundEquip->baseForm->formID & 0xFFFFFF;
		_data.type = GetItemType();

		_data.isEquipped = _data.equipState > 0;
		_data.isStolen = false;

		if (weight > 0) {
			_data.infoWeight = TruncatePrecision(weight);
		}
		if (value > 0 && weight > 0) {
			_data.infoValueWeight = RoundValue(value / weight);
		}

		switch (_foundEquip->baseForm->formType.get()) {
		case RE::FormType::Scroll:
			_data.subTypeDisplay = "$Scroll";

			//_data.magic.duration = TruncatePrecision(_data.magic.duration); // This does nothing for integers
			_data.magic.magnitude = TruncatePrecision(_data.magic.magnitude);
			break;

		case RE::FormType::Armor:
			_data.armor.isEnchanted = _foundEquip->IsEnchanted();
			_data.armor.infoArmor = TruncatePrecision(_foundEquip->baseForm->As<RE::TESObjectARMO>()->armorRating);

			SkyUiProcessArmorClass();
			SkyUiProcessArmorPartMask();
			SkyUiProcessMaterialKeywords();
			SkyUiProcessArmorOther();
			SkyUiProcessArmorOther();
			break;

		case RE::FormType::Book:
			SkyUiProcessBookType();
			break;

		case RE::FormType::Ingredient:
			_data.subTypeDisplay = "$Ingredient";
			break;

		case RE::FormType::Light:
			_data.subTypeDisplay = "$Torch";
			break;

		case RE::FormType::Misc:
			SkyUiProcessMiscType();
			SkyUiProcessMiscKnownForms();
			break;

		case RE::FormType::Weapon:
			_data.weapon.isEnchanted = _foundEquip->IsEnchanted();
			_data.weapon.isPoisoned = _foundEquip->objectData ? _foundEquip->objectData->HasType(RE::ExtraDataType::kPoison) : false;
			_data.weapon.infoDamage = TruncatePrecision(_foundEquip->baseForm->As<RE::TESObjectWEAP>()->attackDamage);

			SkyUiProcessWeaponType();
			SkyUiProcessMaterialKeywords();
			SkyUiProcessWeaponKnownForms();
			break;

		case RE::FormType::Ammo:
			_data.ammo.isEnchanted = _foundEquip->IsEnchanted();
			_data.ammo.infoDamage = TruncatePrecision(_foundEquip->baseForm->As<RE::TESAmmo>()->data.damage);

			SkyUiProcessAmmoType();
			SkyUiProcessMaterialKeywords();
			SkyUiProcessAmmoKnownForms();
			break;

		case RE::FormType::KeyMaster:
			SkyUiProcessKeyType();
			break;

		case RE::FormType::AlchemyItem:
			//_data.magic.duration = TruncatePrecision(_data.magic.duration); // This does nothing for integers
			_data.magic.magnitude = TruncatePrecision(_data.magic.magnitude);

			SkyUiProcessPotionType();
			break;

		case RE::FormType::SoulGem:
			SkyUiProcessSoulGemType();
			SkyUiProcessSoulGemStatus();
			SkyUiProcessSoulGemKnownForms();
			break;

		default:
			break;
		}
	}

	void ItemStack::SkyUiProcessArmorClass() const {
		PROFILE_SCOPE;

		if (const auto armor = skyrim_cast<RE::TESObjectARMO*>(_foundEquip->baseForm)) {
			switch (static_cast<ArmorWeightClass>(armor->bipedModelData.armorType.underlying())) {
			case ArmorWeightClass::kLight:
				_data.armor.weightClassDisplay = "$Light";
				break;

			case ArmorWeightClass::kHeavy:
				_data.armor.weightClassDisplay = "$Heavy";
				break;

			case ArmorWeightClass::kNone:
				if (_foundEquip->baseForm->HasKeywordByEditorID("VendorItemClothing")) {
					_data.armor.weightClass = ArmorWeightClass::kClothing;
					_data.armor.weightClassDisplay = "$Clothing";
					break;
				}

				if (_foundEquip->baseForm->HasKeywordByEditorID("VendorItemJewelry")) {
					_data.armor.weightClass = ArmorWeightClass::kJewelry;
					_data.armor.weightClassDisplay = "$Jewelry";
					break;
				}

				_data.armor.weightClass.unset();
				_data.armor.weightClassDisplay = "$Other";
				break;

			default:
				break;
			}
		}
	}

	void ItemStack::SkyUiProcessArmorPartMask() const {
		PROFILE_SCOPE;

		if (const auto armor = skyrim_cast<RE::TESObjectARMO*>(_foundEquip->baseForm)) {
			const auto partMask = armor->bipedModelData.bipedObjectSlots;
			auto mainPartMask = ArmorSlot::kNone;

			for (auto slot : ArmorSlotPrecedence) {
				if (partMask.all(slot)) {
					mainPartMask = slot;
					break;
				}
			}

			_data.armor.mainPartMask = mainPartMask;

			if (mainPartMask == ArmorSlot::kNone) {
				return;
			}

			for (const auto& partEntry : ArmorSubTypeTable) {
				if (partEntry.slot == mainPartMask) {
					_data.armor.subType = partEntry.subType;
					_data.subTypeDisplay = partEntry.subTypeDisplay;
					return;
				}
			}
		}
	}

	void ItemStack::SkyUiProcessArmorOther() const {
		PROFILE_SCOPE;

		if (_data.armor.weightClass.valid) {
			return;
		}

		// Set fallback weight classes if no keywords were set.
		switch (_data.armor.mainPartMask) {
		case ArmorSlot::kHead:
		case ArmorSlot::kHair:
		case ArmorSlot::kLongHair:
		case ArmorSlot::kBody:
		case ArmorSlot::kHands:
		case ArmorSlot::kForearms:
		case ArmorSlot::kFeet:
		case ArmorSlot::kCalves:
		case ArmorSlot::kShield:
		case ArmorSlot::kTail:
			_data.armor.weightClass = ArmorWeightClass::kClothing;
			_data.armor.weightClassDisplay = "$Clothing";
			break;

		case ArmorSlot::kAmulet:
		case ArmorSlot::kRing:
		case ArmorSlot::kCirclet:
		case ArmorSlot::kEars:
			_data.armor.weightClass = ArmorWeightClass::kJewelry;
			_data.armor.weightClassDisplay = "$Jewelry";
			break;

		default:
			break;
		}
	}

	void ItemStack::SkyUiProcessArmorKnownForms() const {
		PROFILE_SCOPE;

		if (_foundEquip->baseForm == KnownForms::ClothesWeddingWreath) {
			_data.armor.weightClass = ArmorWeightClass::kJewelry;
			_data.armor.weightClassDisplay = "$Jewelry";
		}

		if (_foundEquip->baseForm == KnownForms::DLC1ClothesVampireLordArmor) {
			_data.armor.subType = ArmorSubType::kBody;
			_data.subTypeDisplay = "$Body";
		}
	}

	void ItemStack::SkyUiProcessMaterialKeywords() const {
		PROFILE_SCOPE;

		for (const auto& entry : MaterialTable) {
			if (_foundEquip->baseForm->HasAnyKeywordByEditorID(entry.keywords)) {
				_data.material.material = entry.material;
				_data.material.materialDisplay = entry.materialDisplay;
				return;
			}
		}

		_data.material.materialDisplay = "$Other";
	}

	void ItemStack::SkyUiProcessBookType() const {
		PROFILE_SCOPE;

		if (const auto book = skyrim_cast<RE::TESObjectBOOK*>(_foundEquip->baseForm)) {
			_data.book.isRead = book->data.flags.all(BookFlags::kHasBeenRead);
		}

		if (_foundEquip->baseForm->HasKeywordByEditorID("VendorItemRecipe")) {
			_data.book.subType = BookSubType::kRecipe;
			_data.subTypeDisplay = "$Recipe";
			return;
		}

		if (_foundEquip->baseForm->HasKeywordByEditorID("VendorItemSpellTome")) {
			_data.book.subType = BookSubType::kSpellTome;
			_data.subTypeDisplay = "$Spell Tome";
			return;
		}

		if (_data.book.bookType.value.underlying() == static_cast<uint8_t>(BookType::kNoteScroll)) {
			_data.book.subType = BookSubType::kNote;
			_data.subTypeDisplay = "$Note";
			return;
		}

		_data.book.subType.unset();
		_data.subTypeDisplay = "$Book";
	}

	void ItemStack::SkyUiProcessMiscType() const {
		PROFILE_SCOPE;

		for (const auto& entry : MiscSubTypeTable) {
			if (_foundEquip->baseForm->HasAnyKeywordByEditorID(entry.keywords)) {
				_data.misc.subType = entry.subType;
				_data.subTypeDisplay = entry.subTypeDisplay;
				return;
			}
		}

		_data.misc.subType.unset();
		_data.subTypeDisplay = "$Misc";
	}

	void ItemStack::SkyUiProcessMiscKnownForms() const {
		PROFILE_SCOPE;

		if (_foundEquip->baseForm == KnownForms::Gold001) {
			_data.misc.subType = MiscType::kGold;
			_data.subTypeDisplay = "$Gold";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::Lockpick) {
			_data.misc.subType = MiscType::kLockpick;
			_data.subTypeDisplay = "$Lockpick";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::Leather01) {
			_data.misc.subType = MiscType::kLeather;
			_data.subTypeDisplay = "$Leather";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::LeatherStrips) {
			_data.misc.subType = MiscType::kLeatherStrips;
			_data.subTypeDisplay = "$Strips";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::GemAmethystFlawless) {
			_data.misc.subType = MiscType::kGem;
			_data.subTypeDisplay = "$Gem";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::dunDeadMensRubyDragonClaw ||
			_foundEquip->baseForm == KnownForms::dunFolgunthurIvoryDragonClaw ||
			_foundEquip->baseForm == KnownForms::dunForelhostGlassClaw ||
			_foundEquip->baseForm == KnownForms::dunKorvanjundEbonyDragonClaw ||
			_foundEquip->baseForm == KnownForms::dunReachwaterRockEmeraldDragonClaw ||
			_foundEquip->baseForm == KnownForms::dunSkuldafnDiamondClaw ||
			_foundEquip->baseForm == KnownForms::dunValthumeIronClaw ||
			_foundEquip->baseForm == KnownForms::dunYngolBarrowCoralClaw ||
			_foundEquip->baseForm == KnownForms::E3GoldenClaw ||
			_foundEquip->baseForm == KnownForms::FFI01Claw ||
			_foundEquip->baseForm == KnownForms::MS13GoldenClaw) {
			_data.misc.subType = MiscType::kDragonClaw;
			_data.subTypeDisplay = "$Claw";
			return;
		}
	}

	void ItemStack::SkyUiProcessWeaponType() const {
		PROFILE_SCOPE;

		if (const auto weapon = skyrim_cast<RE::TESObjectWEAP*>(_foundEquip->baseForm)) {
			auto animationType = weapon->weaponData.animationType;
			if (animationType >= WeaponAnimationType::kTotal) {
				animationType -= WeaponAnimationType::kTotal;
			}

			switch (animationType.get()) {
			case WeaponAnimationType::kHandToHandMelee:
				_data.weapon.subType = WeaponType::kMelee;
				_data.subTypeDisplay = "$Melee";
				break;

			case WeaponAnimationType::kOneHandSword:
				_data.weapon.subType = WeaponType::kSword;
				_data.subTypeDisplay = "$Sword";
				break;

			case WeaponAnimationType::kOneHandDagger:
				_data.weapon.subType = WeaponType::kDagger;
				_data.subTypeDisplay = "$Dagger";
				break;

			case WeaponAnimationType::kOneHandAxe:
				_data.weapon.subType = WeaponType::kWarAxe;
				_data.subTypeDisplay = "$War Axe";
				break;

			case WeaponAnimationType::kOneHandMace:
				_data.weapon.subType = WeaponType::kMace;
				_data.subTypeDisplay = "$Mace";
				break;

			case WeaponAnimationType::kTwoHandSword:
				_data.weapon.subType = WeaponType::kGreatsword;
				_data.subTypeDisplay = "$Greatsword";
				break;

			case WeaponAnimationType::kTwoHandAxe:
				if (_foundEquip->baseForm->HasKeywordByEditorID("WeapTypeWarhammer")) {
					_data.weapon.subType = WeaponType::kWarhammer;
					_data.subTypeDisplay = "$Warhammer";
				} else {
					_data.weapon.subType = WeaponType::kBattleaxe;
					_data.subTypeDisplay = "$Battleaxe";
				}
				break;

			case WeaponAnimationType::kBow:
				_data.weapon.subType = WeaponType::kBow;
				_data.subTypeDisplay = "$Bow";
				break;

			case WeaponAnimationType::kStaff:
				_data.weapon.subType = WeaponType::kStaff;
				_data.subTypeDisplay = "$Staff";
				break;

			case WeaponAnimationType::kCrossbow:
				_data.weapon.subType = WeaponType::kCrossbow;
				_data.subTypeDisplay = "$Crossbow";
				break;

			default:
				_data.weapon.subType.unset();
				_data.subTypeDisplay = "$Weapon";
				break;
			}
		}
	}

	void ItemStack::SkyUiProcessWeaponKnownForms() const {
		PROFILE_SCOPE;

		if (_foundEquip->baseForm == KnownForms::weapPickaxe ||
			_foundEquip->baseForm == KnownForms::SSDRocksplinterPickaxe ||
			_foundEquip->baseForm == KnownForms::dunVolunruudPickaxe) {
			_data.weapon.subType = WeaponType::kPickaxe;
			_data.subTypeDisplay = "$Pickaxe";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::Axe01 ||
			_foundEquip->baseForm == KnownForms::dunHaltedStreamPoachersAxe) {
			_data.weapon.subType = WeaponType::kWoodAxe;
			_data.subTypeDisplay = "$Wood Axe";
			return;
		}
	}

	void ItemStack::SkyUiProcessAmmoType() const {
		PROFILE_SCOPE;

		if (const auto ammo = skyrim_cast<RE::TESAmmo*>(_foundEquip->baseForm)) {
			if (ammo->GetRuntimeData().data.flags.all(AmmoFlags::kNonBolt)) {
				_data.ammo.subType = AmmoType::kArrow;
				_data.subTypeDisplay = "$Arrow";
			} else {
				_data.ammo.subType = AmmoType::kBolt;
				_data.subTypeDisplay = "$Bolt";
			}
		}
	}

	void ItemStack::SkyUiProcessAmmoKnownForms() const {
		PROFILE_SCOPE;

		if (_foundEquip->baseForm == KnownForms::DaedricArrow) {
			_data.material.material = MaterialType::kDaedric;
			_data.material.materialDisplay = "$Daedric";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::EbonyArrow) {
			_data.material.material = MaterialType::kEbony;
			_data.material.materialDisplay = "$Ebony";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::GlassArrow) {
			_data.material.material = MaterialType::kGlass;
			_data.material.materialDisplay = "$Glass";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::ElvenArrow ||
			_foundEquip->baseForm == KnownForms::DLC1ElvenArrowBlessed ||
			_foundEquip->baseForm == KnownForms::DLC1ElvenArrowBlood) {
			_data.material.material = MaterialType::kElven;
			_data.material.materialDisplay = "$Elven";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::DwarvenArrow ||
			_foundEquip->baseForm == KnownForms::DwarvenSphereArrow ||
			_foundEquip->baseForm == KnownForms::DwarvenSphereBolt01 ||
			_foundEquip->baseForm == KnownForms::DwarvenSphereBolt02 ||
			_foundEquip->baseForm == KnownForms::DLC2DwarvenBallistaBolt) {
			_data.material.material = MaterialType::kDwarven;
			_data.material.materialDisplay = "$Dwarven";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::OrcishArrow) {
			_data.material.material = MaterialType::kOrcish;
			_data.material.materialDisplay = "$Orcish";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::NordHeroArrow) {
			_data.material.material = MaterialType::kNordic;
			_data.material.materialDisplay = "$Nordic";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::DraugrArrow) {
			_data.material.material = MaterialType::kDraugr;
			_data.material.materialDisplay = "$Draugr";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::FalmerArrow) {
			_data.material.material = MaterialType::kFalmer;
			_data.material.materialDisplay = "$Falmer";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::SteelArrow ||
			_foundEquip->baseForm == KnownForms::MQ101SteelArrow) {
			_data.material.material = MaterialType::kSteel;
			_data.material.materialDisplay = "$Steel";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::IronArrow ||
			_foundEquip->baseForm == KnownForms::CWArrow ||
			_foundEquip->baseForm == KnownForms::CWArrowShort ||
			_foundEquip->baseForm == KnownForms::TrapDart ||
			_foundEquip->baseForm == KnownForms::dunArcherPracticeArrow ||
			_foundEquip->baseForm == KnownForms::dunGeirmundSigdisArrowsIllusion ||
			_foundEquip->baseForm == KnownForms::FollowerIronArrow ||
			_foundEquip->baseForm == KnownForms::TestDLC1Bolt) {
			_data.material.material = MaterialType::kIron;
			_data.material.materialDisplay = "$Iron";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::ForswornArrow) {
			_data.material.material = MaterialType::kHide;
			_data.material.materialDisplay = "$Forsworn";
			return;
		}

		if (_foundEquip->baseForm == KnownForms::DLC2RieklingSpearThrown) {
			_data.material.material = MaterialType::kWood;
			_data.material.materialDisplay = "$Wood";
			_data.subTypeDisplay = "$Spear";
			return;
		}
	}

	void ItemStack::SkyUiProcessKeyType() const {
		PROFILE_SCOPE;

		_data.subTypeDisplay = "$Key";

		if (_data.infoValue <= 0) {
			_data.infoValue.unset();
		}

		if (_data.infoWeight <= 0) {
			_data.infoWeight.unset();
		}
	}

	void ItemStack::SkyUiProcessPotionType() const {
		PROFILE_SCOPE;

		if (const auto alchemyItem = skyrim_cast<RE::AlchemyItem*>(_foundEquip->baseForm)) {
			if (alchemyItem->data.flags.all(AlchemyFlags::kFoodItem)) {
				if (alchemyItem->data.consumptionSound == KnownForms::ITMPotionUse) {
					_data.potion.subType = PotionType::kDrink;
					_data.subTypeDisplay = "$Drink";
				} else {
					_data.potion.subType = PotionType::kFood;
					_data.subTypeDisplay = "$Food";
				}
				return;
			}

			if (alchemyItem->data.flags.all(AlchemyFlags::kPoison)) {
				_data.potion.subType = PotionType::kPoison;
				_data.subTypeDisplay = "$Poison";
				return;
			}

			const auto actorValue = _data.potion.actorValue;

			for (const auto& entry : PotionTypeTable) {
				if (entry.actorValue == actorValue) {
					_data.potion.subType = entry.subType;
					_data.subTypeDisplay = entry.subTypeDisplay;
					return;
				}
			}

			_data.potion.subType = PotionType::kPotion;
			_data.subTypeDisplay = "$Potion";
		}
	}

	void ItemStack::SkyUiProcessSoulGemType() const {
		PROFILE_SCOPE;

		_data.subTypeDisplay = "$Soul Gem";

		if (const auto soulGem = skyrim_cast<RE::TESSoulGem*>(_foundEquip->baseForm)) {
			if (soulGem->soulCapacity) {
				_data.soulGem.subType = static_cast<SoulLevel>(soulGem->soulCapacity.get());
			} else {
				_data.soulGem.subType.unset();
			}
		}
	}

	void ItemStack::SkyUiProcessSoulGemStatus() const {
		PROFILE_SCOPE;

		if (const auto soulGem = skyrim_cast<RE::TESSoulGem*>(_foundEquip->baseForm)) {
			const auto gemLevel = soulGem->soulCapacity;
			const auto soulLevel = soulGem->currentSoul;

			if (!gemLevel || !soulLevel) {
				_data.soulGem.status = SoulGemStatus::kEmpty;
				return;
			}

			if (soulLevel < gemLevel) {
				_data.soulGem.status = SoulGemStatus::kPartial;
				return;
			}

			_data.soulGem.status = SoulGemStatus::kFull;
		}
	}

	void ItemStack::SkyUiProcessSoulGemKnownForms() const {
		PROFILE_SCOPE;

		if (_foundEquip->baseForm == KnownForms::DA01SoulGemAzurasStar ||
			_foundEquip->baseForm == KnownForms::DA01SoulGemBlackStar) {
			_data.soulGem.subType = SoulLevel::kAzura;
		}
	}
}
