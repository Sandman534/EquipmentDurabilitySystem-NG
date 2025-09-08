#pragma once

#include "RE/G/GFxValue.h"

#include "ItemData.h"
#include "ItemDefines.h"
#include "FoundEquipData.h"
#include "Utility.h"

#define PROFILE_SCOPE
#define PROFILE_SCOPE_NAMED(name)

namespace I4Data::Items
{
	class ItemStack
	{
	public:
		ItemStack(ItemStack&&) = delete;
		ItemStack(const ItemStack&) = delete;
		ItemStack& operator=(ItemStack&&) = delete;
		ItemStack& operator=(const ItemStack&) = delete;

		ItemStack(FoundEquipData* foundEquip);

		virtual ~ItemStack() = default;


		[[nodiscard]] virtual ItemData& GetData() const;
		[[nodiscard]] virtual RE::GFxValue& BuildDataObject(RE::GFxMovieView* view) const;

	protected:
		FoundEquipData* _foundEquip;

		mutable ItemData _data{};
		mutable RE::GFxValue _dataObj{};
		mutable bool _dataInitialized = false;

		void SetVanillaData() const;

		void SkseExtendItemData() const;
		void SkseExtendCommonItemData() const;
		void SkseExtendItemInfoData() const;
		void SkseExtendStandardItemData() const;
		void SkseExtendInventoryData() const;
		void SkseExtendMagicItemData() const;

		void SkyUiProcessEntry() const;
		void SkyUiProcessArmorClass() const;
		void SkyUiProcessArmorPartMask() const;
		void SkyUiProcessArmorOther() const;
		void SkyUiProcessArmorKnownForms() const;
		void SkyUiProcessMaterialKeywords() const;
		void SkyUiProcessBookType() const;
		void SkyUiProcessMiscType() const;
		void SkyUiProcessMiscKnownForms() const;
		void SkyUiProcessWeaponType() const;
		void SkyUiProcessWeaponKnownForms() const;
		void SkyUiProcessAmmoType() const;
		void SkyUiProcessAmmoKnownForms() const;
		void SkyUiProcessKeyType() const;
		void SkyUiProcessPotionType() const;
		void SkyUiProcessSoulGemType() const;
		void SkyUiProcessSoulGemStatus() const;
		void SkyUiProcessSoulGemKnownForms() const;

		void SkyUiSelectIcon() const;
		void SkyUiSelectScrollColor() const;
		void SkyUiSelectArmorIcon() const;
		void SkyUiSelectLightArmorIcon() const;
		void SkyUiSelectHeavyArmorIcon() const;
		void SkyUiSelectClothingIcon() const;
		void SkyUiSelectJewelryIcon() const;
		void SkyUiSelectBookIcon() const;
		void SkyUiSelectMiscIcon() const;
		void SkyUiSelectWeaponIcon() const;
		void SkyUiSelectAmmoIcon() const;
		void SkyUiSelectPotionIcon() const;
		void SkyUiSelectSoulGemIcon() const;
		void SkyUiSelectSoulGemStatusIcon(bool grand) const;

		// Helpers
		ItemType GetItemType() const;

		static float RoundValue(float value);
		static float TruncatePrecision(float value);

		EnchantmentType GetEnchantmentType() const;
	};
}
