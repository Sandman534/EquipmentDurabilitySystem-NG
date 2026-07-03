#pragma once

class FoundEquipData {
	public:
		RE::TESForm* baseForm;
		RE::ExtraDataList* objectData;
		RE::TESBoundObject* refForm;
		std::string objectName;
		
		// Regular Generators
		FoundEquipData() : baseForm(nullptr), objectData(nullptr), refForm(nullptr) {}
		explicit FoundEquipData(RE::TESForm* a_Form) : baseForm(a_Form), objectData(nullptr), refForm(nullptr) {}
		explicit FoundEquipData(RE::TESForm* a_Form, RE::ExtraDataList* a_ExtraData) : baseForm(a_Form), objectData(a_ExtraData), refForm(nullptr) {}

		// SetName
		void CreateName();

		// Item Getters
		float GetItemHealthPercent();
		float GetItemHealthForWidget();

		// Item Setters
		void SetItemHealthPercent(float value);
		void SetItemHealthPercentCapped(float value);

		// Item Enchantments
		void SetItemEnchantment(int level, RE::TESObjectREFR* ref);
		const char * GetItemName();

		// Process State
		void ProcessItem();
		bool CanProcess();

		// Status Checks
		bool IsTempered();
		bool IsEnchanted();
		bool IsBroken();
		bool IsUnarmed();
		bool CanBreak();
		bool CanTemper();
		bool CanEnchant();

	private:
		std::optional<GameData::TierInfo> GetTierForLevel(GameData::Material mat, int playerLevel);
		GameData::Material getStrongestMaterial();
		EquipmentType GetEquipmentType();
};

// Additional Equipment functions
FoundEquipData FindEquippedWeapon(RE::InventoryChanges* a_Changes, RE::TESForm* a_Form, bool a_LeftHand);
FoundEquipData FindEquippedArmor(RE::InventoryChanges* a_Changes, RE::BGSBipedObjectForm::BipedObjectSlot a_SlotMask);