#include "ItemStack.h"
#include "ItemDefines.h"

namespace I4Data::Items
{
	ItemStack::ItemStack(FoundEquipData* foundEquip) : _foundEquip(foundEquip) {}

	ItemData& ItemStack::GetData() const {
		if (_dataInitialized) {
			return _data;
		}

		PROFILE_SCOPE;

		SetVanillaData();
		SkseExtendItemData();
		SkyUiProcessEntry();
		SkyUiSelectIcon();

		// MoreHUD enchantment type
		_data.enchantmentType = GetEnchantmentType();

		_dataInitialized = true;
		return _data;
	}

	RE::GFxValue BuildBasicFormInfoObject(RE::GFxMovieView* view, RE::TESForm* form) {
		if (!form) {
			return {};
		}

		RE::GFxValue data;
		view->CreateObject(&data);

		data.SetMember("formType", form->formType.underlying());
		data.SetMember("formId", form->formID);

		if (const auto listForm = skyrim_cast<RE::BGSListForm*>(form)) {
			RE::GFxValue formArray;
			view->CreateArray(&formArray);
			data.SetMember("forms", formArray);

			listForm->ForEachForm([&](RE::TESForm* childForm) {
				formArray.PushBack(BuildBasicFormInfoObject(view, childForm));
				return RE::BSContainer::ForEachResult::kContinue;
			});
		}

		return data;
	}

	RE::GFxValue BuildKeywordsObject(RE::GFxMovieView* view, const RE::BGSKeywordForm* form) {
		if (!form) {
			return {};
		}

		RE::GFxValue keywords;
		view->CreateObject(&keywords);

		for (uint32_t i = 0; i < form->GetNumKeywords(); i++) {
			const auto keyword = form->GetKeywordAt(i).value_or(nullptr);
			if (!keyword)
				continue;

			const auto editorId = keyword->GetFormEditorID();
			if (!editorId || !editorId[0])
				continue;

			keywords.SetMember(editorId, true);
		}

		return keywords;
	}

	RE::GFxValue& ItemStack::BuildDataObject(RE::GFxMovieView* view) const {
		if (_dataObj.IsObject()) {
			return _dataObj;
		}

		const auto data = GetData();

		PROFILE_SCOPE;

		view->CreateObject(&_dataObj);

		SetDataMember(_dataObj, "text", data.text);
		SetDataMember(_dataObj, "count", data.count);
		SetDataMember(_dataObj, "equipState", data.equipState);
		SetDataMember(_dataObj, "filterFlag", data.filterFlag);
		SetDataMember(_dataObj, "favorite", data.favorite);
		SetDataMember(_dataObj, "enabled", data.enabled);
		SetDataMember(_dataObj, "isStealing", data.isStealing);
		SetDataMember(_dataObj, "soulLVL", data.soulLVL);

		SetDataMember(_dataObj, "formType", data.formType);
		SetDataMember(_dataObj, "formId", data.formId);

		SetDataMember(_dataObj, "keywords", BuildKeywordsObject(view, data.keywords));

		SetDataMember(_dataObj, "value", data.value);
		SetDataMember(_dataObj, "weight", data.weight);

		SetDataMember(_dataObj, "baseId", data.baseId);
		SetDataMember(_dataObj, "type", data.type);
		SetDataMember(_dataObj, "isEquipped", data.isEquipped);
		SetDataMember(_dataObj, "isStolen", data.isStolen);

		SetDataMember(_dataObj, "infoValue", data.infoValue);
		SetDataMember(_dataObj, "infoWeight", data.infoWeight);
		SetDataMember(_dataObj, "infoValueWeight", data.infoValueWeight);

		SetDataMember(_dataObj, "subTypeDisplay", data.subTypeDisplay);
		SetDataMember(_dataObj, "iconLabel", data.iconLabel);
		SetDataMember(_dataObj, "iconColor", data.iconColor);

		SetDataMember(_dataObj, "bestInClass", data.bestInClass);

		switch (data.formType.value.get()) {
		case RE::FormType::Armor:
			SetDataMember(_dataObj, "material", data.armor.material);
			SetDataMember(_dataObj, "materialDisplay", data.armor.materialDisplay);
			SetDataMember(_dataObj, "partMask", data.armor.partMask);
			SetDataMember(_dataObj, "weightClass", data.armor.weightClass);
			SetDataMember(_dataObj, "armor", data.armor.armor);
			SetDataMember(_dataObj, "infoArmor", data.armor.infoArmor);
			SetDataMember(_dataObj, "isEnchanted", data.armor.isEnchanted);
			SetDataMember(_dataObj, "weightClassDisplay", data.armor.weightClassDisplay);
			SetDataMember(_dataObj, "mainPartMask", data.armor.mainPartMask);
			SetDataMember(_dataObj, "subType", data.armor.subType);
			break;

		case RE::FormType::Weapon:
			SetDataMember(_dataObj, "material", data.weapon.material);
			SetDataMember(_dataObj, "materialDisplay", data.weapon.materialDisplay);
			SetDataMember(_dataObj, "weaponType", data.weapon.weaponType);
			SetDataMember(_dataObj, "speed", data.weapon.speed);
			SetDataMember(_dataObj, "reach", data.weapon.reach);
			SetDataMember(_dataObj, "stagger", data.weapon.stagger);
			SetDataMember(_dataObj, "critDamage", data.weapon.critDamage);
			SetDataMember(_dataObj, "minRange", data.weapon.minRange);
			SetDataMember(_dataObj, "maxRange", data.weapon.maxRange);
			SetDataMember(_dataObj, "baseDamage", data.weapon.baseDamage);
			SetDataMember(_dataObj, "equipSlot", data.weapon.equipSlot);
			SetDataMember(_dataObj, "damage", data.weapon.damage);
			SetDataMember(_dataObj, "infoDamage", data.weapon.infoDamage);
			SetDataMember(_dataObj, "isPoisoned", data.weapon.isPoisoned);
			SetDataMember(_dataObj, "isEnchanted", data.weapon.isEnchanted);
			SetDataMember(_dataObj, "subType", data.weapon.subType);
			break;

		case RE::FormType::Ammo:
			SetDataMember(_dataObj, "material", data.armor.material);
			SetDataMember(_dataObj, "materialDisplay", data.armor.materialDisplay);
			SetDataMember(_dataObj, "flags", data.ammo.flags);
			SetDataMember(_dataObj, "damage", data.ammo.damage);
			SetDataMember(_dataObj, "infoDamage", data.ammo.infoDamage);
			SetDataMember(_dataObj, "isEnchanted", data.ammo.isEnchanted);
			SetDataMember(_dataObj, "subType", data.ammo.subType);
			break;

		case RE::FormType::AlchemyItem:
			SetDataMember(_dataObj, "flags", data.potion.flags);
			SetDataMember(_dataObj, "useSound", BuildBasicFormInfoObject(view, data.potion.useSound));
			SetDataMember(_dataObj, "subType", data.potion.subType);
			break;

		case RE::FormType::Scroll:
		case RE::FormType::Spell:
			SetDataMember(_dataObj, "spellType", data.spell.spellType);
			SetDataMember(_dataObj, "trueCost", data.spell.trueCost);
			SetDataMember(_dataObj, "equipSlot", data.spell.equipSlot);
			SetDataMember(_dataObj, "effectKeywords", BuildKeywordsObject(view, data.spell.effectKeywords));
			break;

		case RE::FormType::Book:
			SetDataMember(_dataObj, "flags", data.book.flags);
			SetDataMember(_dataObj, "bookType", data.book.bookType);
			SetDataMember(_dataObj, "teachesSpell", data.book.teachesSpell);
			SetDataMember(_dataObj, "teachesSkill", data.book.teachesSkill);
			SetDataMember(_dataObj, "isRead", data.book.isRead);
			SetDataMember(_dataObj, "subType", data.book.subType);
			break;

		case RE::FormType::Misc:
		case RE::FormType::KeyMaster:
			SetDataMember(_dataObj, "subType", data.misc.subType);
			break;

		case RE::FormType::SoulGem:
			SetDataMember(_dataObj, "subType", data.soulGem.subType);
			SetDataMember(_dataObj, "gemSize", data.soulGem.gemSize);
			SetDataMember(_dataObj, "soulSize", data.soulGem.soulSize);
			SetDataMember(_dataObj, "status", data.soulGem.status);
			break;

		default:
			break;
		}

		if (skyrim_cast<RE::MagicItem*>(_foundEquip->baseForm)) {
			SetDataMember(_dataObj, "spellName", data.magic.spellName);
			SetDataMember(_dataObj, "magnitude", data.magic.magnitude);
			SetDataMember(_dataObj, "duration", data.magic.duration);
			SetDataMember(_dataObj, "area", data.magic.area);
			SetDataMember(_dataObj, "effectName", data.magic.effectName);
			SetDataMember(_dataObj, "subType", data.magic.subType);
			SetDataMember(_dataObj, "effectFlags", data.magic.effectFlags);
			SetDataMember(_dataObj, "school", data.magic.school);
			SetDataMember(_dataObj, "skillLevel", data.magic.skillLevel);
			SetDataMember(_dataObj, "archetype", data.magic.archetype);
			SetDataMember(_dataObj, "deliveryType", data.magic.deliveryType);
			SetDataMember(_dataObj, "castTime", data.magic.castTime);
			SetDataMember(_dataObj, "delayTime", data.magic.delayTime);
			SetDataMember(_dataObj, "actorValue", data.magic.actorValue);
			SetDataMember(_dataObj, "castType", data.magic.castType);
			SetDataMember(_dataObj, "resistance", data.magic.resistance);
		}

		return _dataObj;
	}

	void ItemStack::SetVanillaData() const {
		PROFILE_SCOPE;

		// ItemList::Item constructor
		_data.text = _foundEquip->baseForm->GetName();
		_data.count = 1;
		_data.equipState = 0;
		_data.filterFlag = 0;
		_data.favorite = 0;
		_data.enabled = true;

		// InventoryEntryData::PopulateSoulLevel
		if (_foundEquip->baseForm->Is(RE::FormType::SoulGem) && _foundEquip->objectData) {
			_data.soulLVL = static_cast<SoulLevel>(_foundEquip->objectData->GetSoulLevel());
		}

		_data.isStealing = false;
	}

	ItemType ItemStack::GetItemType() const {
		switch (_foundEquip->baseForm->formType.get()) {
		case RE::FormType::Armor:
			return ItemType::kArmor;

		case RE::FormType::Weapon:
		case RE::FormType::Ammo:
			return ItemType::kWeapon;

		case RE::FormType::Book:
			if (const auto book = skyrim_cast<RE::TESObjectBOOK*>(_foundEquip->baseForm)) {
				return book->TeachesSpell() ? ItemType::kMagicItem : ItemType::kBook;
			}
			return ItemType::kBook;

		case RE::FormType::Ingredient:
			return ItemType::kIngredient;

		case RE::FormType::Misc:
		case RE::FormType::Light:
			return ItemType::kMisc;

		case RE::FormType::KeyMaster:
			return ItemType::kKey;

		case RE::FormType::AlchemyItem:
			if (const auto alchemyItem = skyrim_cast<RE::AlchemyItem*>(_foundEquip->baseForm)) {
				return alchemyItem->IsFood() ? ItemType::kFood : ItemType::kMagicItem;
			}
			return ItemType::kMagicItem;

		case RE::FormType::SoulGem:
			return ItemType::kSoulGem;

		default:
			return ItemType::kNone;
		}
	}

	float ItemStack::RoundValue(float value) {
		return value >= 0 ? floorf(value + 0.5f) : ceilf(value - 0.5f);
	}

	float ItemStack::TruncatePrecision(float value) {
		return RoundValue(value * 100) / 100;
	}

	EnchantmentType ItemStack::GetEnchantmentType() const {
		// https://github.com/ahzaab/moreHUDSE/blob/0b6995a8628cec786f822d2e177eae46dcee0569/src/AHZTarget.cpp#L185

		static const auto magicDisallowEnchanting = KnownForms::MagicDisallowEnchanting.LookupForm<RE::BGSKeyword>();

		const auto enchantable = skyrim_cast<RE::TESEnchantableForm*>(_foundEquip->baseForm);
		auto enchantment = enchantable ? enchantable->formEnchanting : nullptr;

		if (const auto reference = _foundEquip->objectData) {
			if (const auto extraEnchantment = reference->GetByType<RE::ExtraEnchantment>()) {
				enchantment = extraEnchantment->enchantment;
			}
		}

		if (!enchantment) {
			return EnchantmentType::kNone;
		}

		if (enchantment->HasKeyword(magicDisallowEnchanting)) {
			return EnchantmentType::kCannotDisenchant;
		}

		if (enchantment->formFlags & RE::TESForm::RecordFlags::kKnown) {
			return EnchantmentType::kKnown;
		}

		if (const auto baseEnchantment = enchantment->data.baseEnchantment) {
			if (baseEnchantment->HasKeyword(magicDisallowEnchanting)) {
				return EnchantmentType::kCannotDisenchant;
			}

			if (baseEnchantment->formFlags & RE::TESForm::RecordFlags::kKnown) {
				return EnchantmentType::kKnown;
			}
		}

		return EnchantmentType::kUnknown;
	}

}
