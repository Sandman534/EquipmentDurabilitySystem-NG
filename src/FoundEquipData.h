#pragma once

struct FoundEquipData
{
	RE::TESForm* pForm;
	RE::ExtraDataList* pExtraData;
	std::string name;

	FoundEquipData() : pForm(nullptr), pExtraData(nullptr)
	{
	}

	explicit FoundEquipData(RE::TESForm* a_pForm) : pForm(a_pForm), pExtraData(nullptr)
	{
	}

	void GenerateName();

	float GetItemHealthPercent();
	void SetItemHealthPercent(float value);

	bool CanBreak();
	bool CanTemper();

	static FoundEquipData FindEquippedWeapon(RE::InventoryChanges *exChanges, bool abLeftHand, RE::TESForm* form);
	static FoundEquipData FindEquippedArmor(RE::InventoryChanges *exChanges, RE::BGSBipedObjectForm::BipedObjectSlot slotMask);
};