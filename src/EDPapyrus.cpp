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

void EDPapyrus::EquipGearByID(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t fixedItemID, std::uint32_t slotID, bool preventUnequip, bool equipSound) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || fixedItemID == 0) return;

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
    GearData FoundGear = GetGearData(*entryLists, item, fixedItemID);
    if (!FoundGear.data) return;

    //=========================================================
    // Process Rest of Equip
    //=========================================================
    RE::BGSEquipSlot * targetEquipSlot = GetEquipSlotById(slotID);
    bool isTargetSlotInUse = false;

    // Need at least 1 (maybe 2 for dual wield, checked later)
    int itemCount = FoundGear.data->GetCount();
    bool hasItemMinCount = itemCount > 0;
    bool canDualWield = false;

    if (hasItemMinCount) {
        // Case 1: Type already equipped in both hands.
        if (FoundGear.data->HasType<RE::ExtraWorn>() && FoundGear.data->HasType<RE::ExtraWornLeft>())
            isTargetSlotInUse = true;

        // Case 2: Type already equipped in right hand.
        else if (FoundGear.data->HasType<RE::ExtraWorn>())
            isTargetSlotInUse = actor->GetEquippedObject(false) || !targetEquipSlot;

        // Case 3: Type already equipped in left hand.
        else if (FoundGear.data->HasType<RE::ExtraWornLeft>())
            isTargetSlotInUse = actor->GetEquippedObject(true);

        // Case 4: Type not equipped yet.
        else
            isTargetSlotInUse = false;
    }

    if (isTargetSlotInUse || !hasItemMinCount) return;

    bool isItemEquipped = FoundGear.data->HasType<RE::ExtraWorn>() || FoundGear.data->HasType<RE::ExtraWornLeft>();

    // Does this item qualify for dual wield?
    if (item->IsWeapon() && targetEquipSlot && isItemEquipped && CanEquipBothHands(actor, item))
        canDualWield = true;

    // Not enough items to dual wield, weapon has to swap hands
    if (canDualWield && itemCount < 2) {
        // If the item is WornLeft or Worn
        if (FoundGear.data->HasType<RE::ExtraWornLeft>() || FoundGear.data->GetByType<RE::ExtraWorn>()) {

			// Unequip might destroy passed list (return value indicates that).
	        if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
		        equipMgr->UnequipObject(actor, FoundGear.item, FoundGear.data, 1, nullptr, true, false, true, false, nullptr);
        }
    }

    // Equip the item
    if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
        equipMgr->EquipObject(actor, FoundGear.item, FoundGear.data, 1, targetEquipSlot, true, false, equipSound, false);
}

bool EDPapyrus::IsGearEquipped(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t fixedItemID, std::uint32_t slotID) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || fixedItemID == 0) return false;

    // Get the player
    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return false;

    // Get the players inventory
    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return false;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return false;

    // Get the Item ID of the gear we have equipped
    GearData FoundEquipped = GetEquippedItem(actor, slotID, fixedItemID);
    if (!FoundEquipped.item) return false;

    // Get the item ID of the gear we plan to equip
    GearData FoundGear = GetGearData(*entryLists, item, fixedItemID);
    if (!FoundGear.item) return false;

    // Are we equipping the same item?
    return
        FoundGear.item->formID == FoundEquipped.item->formID && 
        FoundGear.itemID != FoundEquipped.itemID;
}

std::uint32_t EDPapyrus::FixedItemID(RE::StaticFunctionTag*, RE::TESForm * item, std::uint32_t itemID) {
    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return 0;

    // Grab any container changes
    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return 0;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return 0;

    // Loop through all inventory lists
    for (auto* entry : *entryLists) {
        // Get the object name
        auto* name = entry->GetDisplayName();
        if (!name) continue;

        // Get the object of the item
        RE::TESBoundObject* WorkingObject = entry->GetObject();
        if (!WorkingObject) continue;

        // Hash the display name and compare to the incoming ID.
        // We want to get the item that we are currently trying to get a fixed ID for
        std::uint32_t xItemId = HashItemID(name, WorkingObject->formID);
        if (itemID != xItemId) continue;

        // Set name to the base object, without temper state
        name = WorkingObject->GetName();

        // If the name was customized, grab that instead without the temper state
        if (auto* extraLists = entry->extraLists) {
            for (auto& xList : *extraLists) {
                name = CustomName(xList, WorkingObject->GetName());
            }
        }

        // Return the newly created Item ID
        // This is a more generalized ID that will look for the Object minus temper state
        // If the object is named, that will be in here instead. This way we make sure we
        // are grabbing the correct object
        return HashItemID(name, WorkingObject->formID);
    }

    return 0;
}

std::uint32_t EDPapyrus::UpdatedItemID(RE::StaticFunctionTag*, RE::TESForm* item, std::uint32_t fixedItemID) {
    //=========================================================
    // Status Checks
    //=========================================================
    if (!item || fixedItemID == 0) return 0;

    // Get the player
    auto* utility = Utility::GetSingleton();
    auto* actor = utility->GetPlayer();
    if (!actor) return 0;

    // Get players inventory
    RE::ExtraContainerChanges* containerChanges = static_cast<RE::ExtraContainerChanges*>(actor->extraList.GetByType(RE::ExtraDataType::kContainerChanges));
    if (!containerChanges) return 0;
    auto* entryLists = containerChanges->changes->entryList;
    if (!entryLists) return 0;

    // Get the top item for the associated gear
    GearData FoundGear = GetGearData(*entryLists, item, fixedItemID);
    if (!FoundGear.data) return 0;

    // If the count is greater than 0
    if (FoundGear.data->GetCount() <= 0) return 0;

    // Generate Standard Item ID
    const char* name = FoundGear.data->GetDisplayName(FoundGear.item);
    if (!name) name = FoundGear.item->GetName();
    if (name && std::string_view(name) != "") return HashItemID(name, FoundGear.item->formID);

    return 0;
}

EDPapyrus::GearData EDPapyrus::GetGearData(RE::BSSimpleList<RE::InventoryEntryData*> &entryLists, RE::TESForm* item, std::uint32_t fixedItemID) {
    // If the item name is generic, we will store all matching extralists for further processing
    std::vector<RE::ExtraDataList*> FoundLists;
    RE::TESBoundObject* FoundItem;

    //=========================================================
    // Get Associated Extra Lists
    //=========================================================
    // Loop through inventory
    for (auto* entry : entryLists) {
        // If the item is not favorited, exclude it
        if (!entry->IsFavorited()) continue;
        
        // If the item matches
        if (!entry->GetObject()) continue;
        auto *WorkingItem = entry->GetObject();
        if (WorkingItem->formID != item->formID) continue;
        
        // If the name was customized, grab that instead
        if (auto* extraLists = entry->extraLists) {
            for (auto* xList : *extraLists) {
                // Set name to the base object, without temper status
                auto* name = CustomName(xList, WorkingItem->GetName());

                // If we have found a matching record, store it
                std::uint32_t xItemId = HashItemID(name, WorkingItem->formID);
                if (fixedItemID == xItemId) {
                    FoundItem = entry->GetObject();
                    FoundLists.push_back(xList);
                }
            }
        }
    }

    // If no objects found, return nothing
    if (FoundLists.size() <= 0) return GearData { nullptr, nullptr, 0 };

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

    //=========================================================
    // Generate a Standard Item ID for comparison
    //=========================================================
    std::uint32_t FoundItemID = 0;
    auto* FoundList = FoundLists.back();
    if (FoundList && FoundList->HasType<RE::ExtraTextDisplayData>())
        FoundItemID = HashItemID(FoundList->GetExtraTextDisplayData()->displayName.c_str(), FoundItem->formID);
    else
        FoundItemID = HashItemID(item->GetName(), FoundItem->formID);

    //=========================================================
    // Return the found data
    //=========================================================
    return GearData { FoundItem, FoundList, FoundItemID };    
}

EDPapyrus::GearData EDPapyrus::GetEquippedItem(RE::PlayerCharacter* actor, std::uint32_t slotID, std::uint32_t fixedItemID) {
    // Get the equipped item
    RE::TESForm* EquippedItem = actor->GetEquippedObject(slotID);
    RE::InventoryEntryData* EquippedEntry = actor->GetEquippedEntryData(slotID);
    RE::BSSimpleList<RE::InventoryEntryData*> EquippedEntries;

    // If right hand is null, check for a Two Handed weapon
    if (!EquippedItem && slotID == 1) {
        RE::TESForm* TwoHandedItem = actor->GetEquippedObject(0);
        RE::InventoryEntryData* TwoHandedEntry = actor->GetEquippedEntryData(0);

        if (TwoHandedItem) {
            if (auto* weap = TwoHandedItem->As<RE::TESObjectWEAP>()) {
                if (weap->IsTwoHandedAxe() || weap->IsTwoHandedSword())
                    EquippedItem = TwoHandedItem;
                    EquippedEntry = TwoHandedEntry;
            }
        }
    }

    // If no item is found, return nothing
    if (!EquippedItem || !EquippedEntry) 
        return GearData { nullptr, nullptr, 0 };

    // Add that inventory entry to an inventory list
    EquippedEntries.push_front(EquippedEntry);

    // Return the Gear Data
    return GetGearData(EquippedEntries, EquippedItem, fixedItemID);
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

const char* EDPapyrus::CustomName(RE::ExtraDataList* list, const char* name) {
    const char* NewName = name;
    if (list && list->HasType<RE::ExtraTextDisplayData>()) {
        auto* xTextData = list->GetByType<RE::ExtraTextDisplayData>();
        if (xTextData->IsPlayerSet()) {
            std::string customName = xTextData->displayName.c_str();
            customName.erase(customName.begin() + xTextData->customNameLength, customName.end());
            NewName = customName.c_str();
        }                        
    }
    
    return NewName;
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