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

	// Naming
	void CreateName();
	void SetBrokenName();
	void SetFixedName();

	// Item Getters
	float GetItemHealthPercent();
	float GetItemHealthForWidget();
	float GetItemHealthRounded();

	// Item Setters
	void SetItemHealthPercent(float value);
	void SetItemEnchantment(int level);
	
	// Status Checks
	bool IsTempered();
	bool IsEnchanted();
	bool IsBroken();
	bool CanBreak();
	bool CanTemper();

private:
	int GetRandom(int a, int b);
	int GetEnchantmentListSize();
	std::string GetType();
	float RoundTo5Decimals(float value);
	float Truncate3(float value);
};

FoundEquipData FindEquippedWeapon(RE::InventoryChanges* a_Changes, RE::TESForm* a_Form, bool a_LeftHand);
FoundEquipData FindEquippedArmor(RE::InventoryChanges* a_Changes, RE::BGSBipedObjectForm::BipedObjectSlot a_SlotMask);