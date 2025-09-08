#include "ItemStack.h"
#include "ItemDefines.h"

namespace I4Data::Items {
	void ItemStack::SkseExtendItemData() const {
		PROFILE_SCOPE;

		SkseExtendCommonItemData();
		SkseExtendItemInfoData();
		SkseExtendStandardItemData();
		SkseExtendInventoryData();
		SkseExtendMagicItemData();

		// SKSE also invokes inventory plugins registered via its RegisterScaleformInventory function,
		// but we don't have access to those. It seems to be a pretty rarely used feature though.
	}

	void ItemStack::SkseExtendCommonItemData() const {
		PROFILE_SCOPE;

		_data.formType = _foundEquip->baseForm->formType;
		_data.formId = _foundEquip->baseForm->formID;
	}

	void ItemStack::SkseExtendItemInfoData() const {
		PROFILE_SCOPE;

		_data.value = _foundEquip->baseForm->GetGoldValue();
		_data.weight = _foundEquip->baseForm->GetWeight();
	}

	void ItemStack::SkseExtendStandardItemData() const {
		PROFILE_SCOPE;

		switch (_foundEquip->baseForm->formType.get()) {
		case RE::FormType::Armor:
			if (const auto armor = skyrim_cast<RE::TESObjectARMO*>(_foundEquip->baseForm)) {
				_data.armor.partMask = armor->bipedModelData.bipedObjectSlots;
				_data.armor.weightClass = static_cast<ArmorWeightClass>(armor->bipedModelData.armorType.get());
			}
			break;

		case RE::FormType::Ammo:
			if (const auto ammo = skyrim_cast<RE::TESAmmo*>(_foundEquip->baseForm)) {
				_data.ammo.flags = ammo->GetRuntimeData().data.flags;
			}
			break;

		case RE::FormType::Weapon:
			if (const auto weapon = skyrim_cast<RE::TESObjectWEAP*>(_foundEquip->baseForm)) {
				_data.weapon.weaponType = weapon->weaponData.animationType;
				_data.weapon.speed = weapon->weaponData.speed;
				_data.weapon.reach = weapon->weaponData.reach;
				_data.weapon.stagger = weapon->weaponData.staggerValue;
				_data.weapon.critDamage = weapon->criticalData.damage;
				_data.weapon.minRange = weapon->weaponData.minRange;
				_data.weapon.maxRange = weapon->weaponData.maxRange;
				_data.weapon.baseDamage = weapon->GetAttackDamage();

				if (const auto equipSlot = weapon->GetEquipSlot()) {
					_data.weapon.equipSlot = equipSlot->formID;
				}
			}
			break;

		case RE::FormType::SoulGem:
			if (const auto soulGem = skyrim_cast<RE::TESSoulGem*>(_foundEquip->baseForm)) {
				_data.soulGem.gemSize = static_cast<SoulLevel>(soulGem->soulCapacity.get());
				if (_foundEquip->objectData)
					_data.soulGem.soulSize = static_cast<SoulLevel>(_foundEquip->objectData->GetSoulLevel());
			}
			break;

		case RE::FormType::AlchemyItem:
			if (const auto alchemyItem = skyrim_cast<RE::AlchemyItem*>(_foundEquip->baseForm)) {
				_data.potion.flags = alchemyItem->data.flags;
			}
			break;

		case RE::FormType::Book:
			if (const auto book = skyrim_cast<RE::TESObjectBOOK*>(_foundEquip->baseForm)) {
				_data.book.flags = book->data.flags;
				_data.book.bookType = book->data.type;

				if (book->data.flags.all(BookFlags::kTeachesSpell)) {
					_data.book.teachesSpell = book->data.teaches.spell ? book->data.teaches.spell->formID : -1;
				} else if (book->data.flags.all(BookFlags::kAdvancesActorValue)) {
					_data.book.teachesSkill = book->data.teaches.actorValueToAdvance;
				}

				// We deviate from the SKSE implementation here to incorporate the fix by
				// https://www.nexusmods.com/skyrimspecialedition/mods/32561. It's usually
				// invoked as an SKSE Scaleform plugin, but we can't access it that way.

				static auto notePattern = std::regex(R"(^.*(?:Note|FishMap)[^\\\/]+$)", std::regex_constants::icase);
				if (book->inventoryModel && std::regex_match(book->inventoryModel->GetModel(), notePattern)) {
					_data.book.bookType = BookType::kNoteScroll;
				}
			}
			break;

		default:
			break;
		}
	}

	void ItemStack::SkseExtendInventoryData() const {
		PROFILE_SCOPE;

		if (const auto keywordForm = skyrim_cast<RE::BGSKeywordForm*>(_foundEquip->baseForm))
		{
			_data.keywords = keywordForm;
		}

		const auto player = RE::PlayerCharacter::GetSingleton();

		switch (_foundEquip->baseForm->formType.get()) {
		case RE::FormType::Armor:
			_data.armor.armor = _foundEquip->baseForm->As<RE::TESObjectARMO>()->armorRating;
			break;

		case RE::FormType::Weapon:
			_data.weapon.damage = _foundEquip->baseForm->As<RE::TESObjectWEAP>()->attackDamage;
			break;

		case RE::FormType::Ammo:
			_data.ammo.damage = _foundEquip->baseForm->As<RE::TESAmmo>()->data.damage;
			break;

		default:
			break;
		}
	}

	void ItemStack::SkseExtendMagicItemData() const {
		PROFILE_SCOPE;

		if (const auto magicItem = skyrim_cast<RE::MagicItem*>(_foundEquip->baseForm)) {
			_data.magic.spellName = magicItem->fullName.c_str();

			const auto effect = magicItem->GetCostliestEffectItem(static_cast<RE::MagicSystem::Delivery>(5), false);
			if (effect && effect->baseEffect) {
				_data.magic.magnitude = effect->effectItem.magnitude;
				_data.magic.duration = effect->effectItem.duration;
				_data.magic.area = effect->effectItem.area;

				const auto baseEffect = effect->baseEffect;
				_data.magic.effectName = baseEffect->fullName.c_str();
				_data.magic.subType = baseEffect->data.associatedSkill;
				_data.magic.effectFlags = baseEffect->data.flags;
				_data.magic.school = baseEffect->data.associatedSkill;
				_data.magic.skillLevel = baseEffect->data.minimumSkill;
				_data.magic.archetype = baseEffect->data.archetype;
				_data.magic.deliveryType = baseEffect->data.delivery;
				_data.magic.castTime = baseEffect->data.spellmakingChargeTime;
				_data.magic.delayTime = baseEffect->data.aiDelayTimer;
				_data.magic.actorValue = baseEffect->data.primaryAV;
				_data.magic.castType = baseEffect->data.castingType;
				_data.magic.resistance = baseEffect->data.resistVariable;  // SkyUI renames this from magicType to resistance
			}
		}

		if (const auto spellItem = skyrim_cast<RE::SpellItem*>(_foundEquip->baseForm)) {
			_data.spell.spellType = spellItem->data.spellType;
			_data.spell.trueCost = spellItem->data.costOverride;

			if (const auto equipSlot = spellItem->GetEquipSlot()) {
				_data.spell.equipSlot = equipSlot->formID;
			}

			const auto effect = spellItem->GetCostliestEffectItem(static_cast<RE::MagicSystem::Delivery>(5), false);
			if (effect && effect->baseEffect) {
				if (const auto keywordForm = skyrim_cast<RE::BGSKeywordForm*>(effect->baseEffect)) {
					_data.spell.effectKeywords = keywordForm;
				}
			}
		}

		if (const auto alchemyItem = skyrim_cast<RE::AlchemyItem*>(_foundEquip->baseForm)) {
			if (const auto sound = alchemyItem->data.consumptionSound) {
				_data.potion.useSound = sound;
			}
		}

		if (const auto enchantmentItem = skyrim_cast<RE::EnchantmentItem*>(_foundEquip->baseForm)) {
			_data.enchantment.flags = enchantmentItem->formFlags;
			_data.enchantment.baseEnchant = enchantmentItem->data.baseEnchantment;
			_data.enchantment.restrictions = enchantmentItem->data.wornRestrictions;
		}

		if (const auto shout = skyrim_cast<RE::TESShout*>(_foundEquip->baseForm)) {
			_data.shout.fullName = shout->fullName.c_str();

			for (int i = 0; i < 3; ++i) {
				const auto& shoutWord = shout->variations[i];
				_data.shout.words[i].word = shoutWord.word->fullName.c_str();
				_data.shout.words[i].fullName = shoutWord.word->translation.c_str();
				_data.shout.words[i].recoveryTime = shoutWord.recoveryTime;
			}
		}
	}
}
