#undef GetObject
#include "EDPapyrus.h"
#include "Utility.h"

bool EDPapyrus::Register(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("EquipGearbyID", ScriptName, EquipGearByID);
    vm->RegisterFunction("IsGearEquipped", ScriptName, IsGearEquipped);
    vm->RegisterFunction("FixedItemID", ScriptName, FixedItemID);
    vm->RegisterFunction("UpdatedItemID", ScriptName, UpdatedItemID);

    return true;
}

void EDPapyrus::EquipGearByID(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t itemID, std::uint32_t slotID, bool preventUnequip, bool equipSound) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || itemID == 0) return;

    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return;

    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return;

    //=========================================================
    // Get the top item for the associated gear
    //=========================================================
    GearData FoundGear = GetGearData(entryLists, item, itemID);
    if (!FoundGear.FoundData) return;

    //=========================================================
    // Process Rest of Equip
    //=========================================================
    RE::BGSEquipSlot * targetEquipSlot = GetEquipSlotById(slotID);
    bool isTargetSlotInUse = false;


    // Need at least 1 (maybe 2 for dual wield, checked later)
    int itemCount = FoundGear.FoundData->GetCount();
    bool hasItemMinCount = itemCount > 0;
    bool canDualWield = false;

    // RE::ExtraDataList* newEquipList = FoundData;

    if (hasItemMinCount)
    {
        // Case 1: Type already equipped in both hands.
        if (FoundGear.FoundData->HasType<RE::ExtraWorn>() && FoundGear.FoundData->HasType<RE::ExtraWornLeft>())
            isTargetSlotInUse = true;

        // Case 2: Type already equipped in right hand.
        else if (FoundGear.FoundData->HasType<RE::ExtraWorn>())
            isTargetSlotInUse = actor->GetEquippedObject(false) || targetEquipSlot == NULL;

        // Case 3: Type already equipped in left hand.
        else if (FoundGear.FoundData->HasType<RE::ExtraWornLeft>())
            isTargetSlotInUse = actor->GetEquippedObject(true);

        // Case 4: Type not equipped yet.
        else
            isTargetSlotInUse = false;
    }

    if (isTargetSlotInUse || !hasItemMinCount) return;

    bool isItemEquipped = FoundGear.FoundData->HasType<RE::ExtraWorn>() || FoundGear.FoundData->HasType<RE::ExtraWornLeft>();

    // Does this item qualify for dual wield?
    if (item->IsWeapon() && targetEquipSlot && isItemEquipped && CanEquipBothHands(actor, item))
        canDualWield = true;

    // Not enough items to dual wield, weapon has to swap hands
    if (canDualWield && itemCount < 2) {
        // If the item is WornLeft or Worn
        if (FoundGear.FoundData->HasType<RE::ExtraWornLeft>() || FoundGear.FoundData->GetByType<RE::ExtraWorn>()) {

			// Unequip might destroy passed list (return value indicates that).
	        if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
		        equipMgr->UnequipObject(actor, FoundGear.FoundItem, FoundGear.FoundData, 1, nullptr, true, false, true, false, nullptr);
        }
    }

    // Equip the item
    if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
        equipMgr->EquipObject(actor, FoundGear.FoundItem, FoundGear.FoundData, 1, targetEquipSlot, true, false, equipSound, false);

}

bool EDPapyrus::IsGearEquipped(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t itemID, std::uint32_t slotID) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || itemID == 0) return false;

    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return false;

    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return false;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return false;

    RE::TESForm* EquippedItem = GetEquippedItem(actor, slotID);
    if (!GetEquippedItem) return false;

    const char* name = EquippedItem->GetName();
    std::uint32_t formID = EquippedItem->formID;

    std::uint32_t EquippedItemID = HashItemID(EquippedItem->GetName(), EquippedItem->formID);
    GearData FoundGear = GetGearData(entryLists, item, itemID);

    return item->GetFormID() == EquippedItem->GetFormID() && FoundGear.FoundItemID != EquippedItemID;
}

std::uint32_t EDPapyrus::FixedItemID(RE::StaticFunctionTag*, RE::TESForm * item, std::uint32_t itemID) {
    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return 0;

    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return 0;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return 0;

    // Loop through 
    for (auto* entry : *entryLists) {
        if (auto* name = entry->GetDisplayName()) {

            // Hash the display name of the object, compare it to the incoming ID
            std::uint32_t xItemId = HashItemID(name, entry->GetObject()->formID);
            if (itemID == xItemId) {
                // Set name to the base object, without temper status
                name = entry->GetObject()->GetName();

                // If the name was customized, grab that instead
                if (auto* extraLists = entry->extraLists) {
                    for (auto& xList : *extraLists) {
                        if (xList && xList->HasType<RE::ExtraTextDisplayData>()) {
                            auto* xTextData = xList->GetByType<RE::ExtraTextDisplayData>();
                            if (xTextData->IsPlayerSet()) {
                                std::string customName = xTextData->displayName.c_str();
                                customName.erase(customName.begin() + xTextData->customNameLength, customName.end());
                                name = customName.c_str();
                            }                        
                        }
                    }
                }

                // return the newly created Item ID
                return HashItemID(name, entry->GetObject()->formID);
            }
        }
    }

    return 0;
}

std::uint32_t EDPapyrus::UpdatedItemID(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t itemID) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || itemID == 0) return 0;

    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return 0;

    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return 0;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return 0;

    //=========================================================
    // Get the top item for the associated gear
    //=========================================================
    GearData FoundGear = GetGearData(entryLists, item, itemID);
    if (!FoundGear.FoundData) return 0;

    //=========================================================
    // If the count is greater than 0
    //=========================================================
    if (FoundGear.FoundData->GetCount() <= 0) return 0;

    //=========================================================
    // Generate Standard Item ID
    //=========================================================
    const char* name = FoundGear.FoundData->GetDisplayName(FoundGear.FoundItem);

    if (!name) 
        name = FoundGear.FoundItem->GetName();

    if (name && name != "") 
        return HashItemID(name, FoundGear.FoundItem->formID);

    return 0;
}

EDPapyrus::GearData EDPapyrus::GetGearData(RE::BSSimpleList<RE::InventoryEntryData *> *entryLists, RE::TESForm* item, std::uint32_t itemID) {
    // If the item name is generic, we will store all matching extralists for further processing
    std::vector<RE::ExtraDataList*> FoundLists;
    RE::TESBoundObject* FoundItem;

    //=========================================================
    // Get Associated Extra Lists
    //=========================================================
    // Loop through inventory
    for (auto* entry : *entryLists) {
        if (!entry->IsFavorited()) continue;
        if (entry->GetObject()->formID != item->formID) continue;

        FoundItem = entry->GetObject();
        // If the name was customized, grab that instead
        if (auto* extraLists = entry->extraLists) {
            for (auto* xList : *extraLists) {
                // Set name to the base object, without temper status
                auto* name = entry->GetObject()->GetName();

                if (xList && xList->HasType<RE::ExtraTextDisplayData>()) {
                    auto* xTextData = xList->GetByType<RE::ExtraTextDisplayData>();
                    if (xTextData->IsPlayerSet()) {
                        std::string customName = xTextData->displayName.c_str();
                        customName.erase(customName.begin() + xTextData->customNameLength, customName.end());
                        name = customName.c_str();
                    }
                }

                // If we have found a matching record, store it
                std::uint32_t xItemId = HashItemID(name, entry->GetObject()->formID);
                if (itemID == xItemId) FoundLists.push_back(xList);
            }
        }
    }

    // If no objects found, return nothing
    if (FoundLists.size() <= 0)
        return GearData { nullptr, nullptr, 0 };

    //=========================================================
    // Process found ExtraLists
    //=========================================================
    // Sort by object Charge
    std::sort(FoundLists.begin(), FoundLists.end(), [](RE::ExtraDataList* const a, RE::ExtraDataList* const b) {
        return GetObjectCharge(a) > GetObjectCharge(b);
    });

    // Sort by Object Temper
    std::stable_sort(FoundLists.begin(), FoundLists.end(), [](RE::ExtraDataList* const a, RE::ExtraDataList* const b) { 
        return GetObjectHealth(a) > GetObjectHealth(b);
    });

    // Gathered Found List and ItemID
    std::uint32_t FoundItemID = 0;
    auto* FoundList = FoundLists.back();
    if (FoundList && FoundList->HasType<RE::ExtraTextDisplayData>())
        FoundItemID = HashItemID(FoundList->GetExtraTextDisplayData()->displayName.c_str(), FoundItem->formID);
    else
        FoundItemID = HashItemID(item->GetName(), FoundItem->formID);

    return GearData { FoundItem, FoundList, FoundItemID };    
}

float EDPapyrus::GetObjectHealth(RE::ExtraDataList* extralist) {
    auto* xhealth = extralist ? extralist->GetByType<RE::ExtraHealth>() : nullptr;
    return xhealth ? xhealth->health : 1.f;
}

float EDPapyrus::GetObjectCharge(RE::ExtraDataList* extralist) {
    auto* xench = extralist ? extralist->GetByType<RE::ExtraEnchantment>() : nullptr;
    if (!xench) {
        return -std::numeric_limits<float>::infinity();
    }
    auto* xcharge = extralist->GetByType<RE::ExtraCharge>();
    return xcharge ? xcharge->charge : static_cast<float>(xench->charge);
}

RE::TESForm* EDPapyrus::GetEquippedItem(RE::PlayerCharacter* actor, std::uint32_t slotID) {
    // Get the equipped item
    RE::TESForm* EquippedItem = actor->GetEquippedObject(slotID);

    // If right hand is null, check for a Two Handed weapon
    if (!EquippedItem && slotID == 1) {
        RE::TESForm* TwoHandEquipItem = actor->GetEquippedObject(0);

        if (TwoHandEquipItem) {
            if (auto* weap = TwoHandEquipItem->As<RE::TESObjectWEAP>()) {
                if (weap->IsTwoHandedAxe() || weap->IsTwoHandedSword())
                    return TwoHandEquipItem;
            }
        }
    }

    return EquippedItem;
}

RE::BGSEquipSlot* EDPapyrus::GetEquipSlotById(std::uint32_t slotID) {
	enum {
		kSlotId_Default = 0,
		kSlotId_Right = 1,
		kSlotId_Left = 2
	};

    auto* utility = Utility::GetSingleton();

	if (slotID == kSlotId_Right) return utility->RightHand;
	else if (slotID == kSlotId_Left) return utility->LeftHand;
	else return nullptr;
}

bool EDPapyrus::CanEquipBothHands(RE::Actor* actor, RE::TESForm * item) {
	RE::BGSEquipType* equipType = item->As<RE::TESObjectWEAP>();
	if (!equipType) return false;

	RE::BGSEquipSlot* equipSlot = equipType->GetEquipSlot();
	if (!equipSlot) return false;

    auto* utility = Utility::GetSingleton();

	// 2 Handed
	if (equipSlot == utility->EitherHand)
		return true;

	// 1 Handed
	else if (equipSlot == utility->LeftHand || equipSlot == utility->RightHand)
		return (actor->GetRace()->data.flags & RE::RACE_DATA::Flag::kCanDualWield) && item->IsWeapon();

	return false;
}

std::uint32_t EDPapyrus::HashItemID(const char * name, std::uint32_t formID) {
    // light plugin
	if((formID >> 24) == 0xFE)
		formID &= 0x0FFF;
	else
		formID &= 0x00FFFFFF;

	return CRC32(name, formID);
}

std::uint32_t EDPapyrus::CRC32(const char* str, std::uint32_t start) {
    std::uint32_t result = ~start;
    unsigned char* c = (unsigned char*) str;
    size_t len = strlen(str);
    while (len-- > 0)
        result = (result >> 8) ^ EDPapyrus::s_crc32Lookup[(result & 0xFF) ^ *c++];
    return ~result;
}