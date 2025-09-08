#undef GetObject
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"

void FoundEquipData::CreateName() {
    // Specificall create unarmed
	if (IsUnarmed()) {
		objectName = "Unarmed";
		return;
	}

    // Process normally
	if (!baseForm || !objectData) return;

    // Resolve the extra data to an object to pull information about it
    RE::InventoryEntryData entry(baseForm->As<RE::TESBoundObject>(), objectData->GetCount());
    entry.AddExtraList(objectData);

	// Get the basic name
    objectName = (IsUnarmed()) ? "Unarmed" : std::string(entry.GetDisplayName());

    // Append poison name if applicable
	if (baseForm->IsWeapon() && Settings::GetSingleton()->ED_Widget_ShowPoisonName && objectData->HasType(RE::ExtraDataType::kPoison)) {
        if (auto* xPoison = static_cast<RE::ExtraPoison*>(objectData->GetByType(RE::ExtraDataType::kPoison))) {
            if (xPoison->poison) objectName += " " + std::string(xPoison->poison->GetFullName()) + " (" + std::to_string(xPoison->count) + ")";
        }
    }
}

void FoundEquipData::SetBrokenName() {
	if (!objectData) return;

    // Get the text data
    RE::ExtraTextDisplayData* textData = objectData->GetExtraTextDisplayData();
	if (!textData) return;

    // Get the current name and remove any 
    std::string oldName = textData->displayName.c_str();

    // Remove any tempering name from the object
	if (GetItemHealthPercent() >= 1.1f) {
		std::size_t pos = oldName.rfind('(');
		if (pos != std::string::npos) {
			oldName = oldName.substr(0, pos);
			if (!oldName.empty() && oldName.back() == ' ')
				oldName.pop_back();
		}
	}

    // Set the name and prefix it with the broken attribute
	std::string newName = "[B]";
	newName += oldName;

    // Assign back to displayName (BSFixedString)
	textData->SetName(newName.c_str());

}

void FoundEquipData::SetFixedName() {
	if (!baseForm || !objectData) return;

    // Get the text data
    RE::ExtraTextDisplayData* textData = objectData->GetExtraTextDisplayData();
	if (!textData) return;

    // Create a new string with the prefix
    std::string name = textData->displayName.c_str();

    // Find and remove "[B]"
    const std::string brokenTag = "[B]";
    size_t pos = name.find(brokenTag);
    if (pos != std::string::npos)
        name.erase(pos, brokenTag.length());

    // Assign back to displayName (BSFixedString)
	textData->SetName(name.c_str());

}

float FoundEquipData::GetItemHealthForWidget() {
	if (IsUnarmed()) return 0.0f;
	if (!objectData) return Utility::GetSingleton()->DefaultWidgetHealth() + 0.001f;
	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return TruncateToDecimals(xHealth->health,3) + Utility::GetSingleton()->StepToMin + 0.001f;
	return Utility::GetSingleton()->DefaultWidgetHealth() + 0.001f;
}

float FoundEquipData::GetItemHealthPercent() {
	if (!objectData) return Utility::GetSingleton()->DefaultHealth();
    if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return xHealth->health;
	return Utility::GetSingleton()->DefaultHealth();
}

float FoundEquipData::GetItemHealthRounded() {
    if (!objectData) return Utility::GetSingleton()->DefaultHealth();
	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return RoundTo5Decimals(xHealth->health);
	return Utility::GetSingleton()->DefaultHealth();
}

void FoundEquipData::SetItemHealthPercent(float value) {
	if (!objectData || IsUnarmed()) return;

    auto* extraHealth = objectData->GetByType<RE::ExtraHealth>();
    if (!extraHealth) {
        extraHealth = static_cast<RE::ExtraHealth*>(
            RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address())
        );
        objectData->Add(extraHealth);
    }
    extraHealth->health = RoundTo5Decimals(value);
}

void FoundEquipData::SetItemEnchantment(int level) {
	if (!objectData) return;

    // Round a random value between 100 and 500 to the nearest 100
    auto roundTo100 = [](int value) {
        int remainder = value % 100;
        if (remainder >= 50) return value + 100 - remainder;
        if (remainder != 0) return value - remainder;
        return value;
    };
    int newValue = roundTo100(GetRandom(100, 500));

	// Determine magnitude threshold based on level
    float magMax = 70.0f;
    if (level <= 8) magMax = 12.5f;
    else if (level <= 16) magMax = 15.0f;
    else if (level <= 24) magMax = 20.0f;
    else if (level <= 32) magMax = 30.0f;
    else if (level <= 40) magMax = 45.0f;

    // Filter enchantments by magnitude
    auto* tempEnch = Settings::GetSingleton()->GetEnchantmentList(GetType());
    std::vector<Enchantments> validEnch;
    std::copy_if(tempEnch->begin(), tempEnch->end(), std::back_inserter(validEnch), [magMax](const Enchantments& e) { return e.enchantment->effects[0]->GetMagnitude() <= magMax; });

    if (validEnch.empty()) return;

	// Pick a random enchantment
    Enchantments selectEnch = validEnch.at(GetRandom(0, validEnch.size() - 1));
	
	// Apply or create ExtraEnchantment
    auto* xEnch = static_cast<RE::ExtraEnchantment*>(objectData->GetByType(RE::ExtraDataType::kEnchantment));
    if (!xEnch) {
        xEnch = static_cast<RE::ExtraEnchantment*>(RE::ExtraEnchantment::Create(sizeof(RE::ExtraEnchantment), RE::VTABLE_ExtraEnchantment[0].address()));
        objectData->Add(xEnch);
    }
    xEnch->enchantment = selectEnch.enchantment;
    xEnch->charge = newValue;
	
    // Set item name with enchantment
    objectName = std::string(baseForm->GetName()) + " " + selectEnch.name;

    auto* xTextData = static_cast<RE::ExtraTextDisplayData*>(objectData->GetByType(RE::ExtraDataType::kTextDisplayData));
    if (!xTextData) {
        xTextData = static_cast<RE::ExtraTextDisplayData*>(RE::ExtraTextDisplayData::Create(sizeof(RE::ExtraTextDisplayData), RE::VTABLE_ExtraTextDisplayData[0].address()));
        objectData->Add(xTextData);
    }
    xTextData->SetName(objectName.c_str());
}

bool FoundEquipData::IsTempered() {
	return objectData 
        && objectData->GetByType<RE::ExtraHealth>() != nullptr 
        && objectData->GetByType<RE::ExtraHealth>()->health > Utility::GetSingleton()->DefaultHealth();
}

bool FoundEquipData::HasBeenProcessed() {
	return objectData && objectData->GetByType<RE::ExtraHealth>() != nullptr;
}

void FoundEquipData::ProcessItem() {
    if (!objectData || IsUnarmed()) return;

    auto* extraHealth = objectData->GetByType<RE::ExtraHealth>();
    if (!extraHealth) {
        extraHealth = static_cast<RE::ExtraHealth*>(
            RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address())
        );
        objectData->Add(extraHealth);
    }
    extraHealth->health = RoundTo5Decimals(Utility::GetSingleton()->DefaultHealth());
}

bool FoundEquipData::IsEnchanted() {
	return objectData && (
		(baseForm && baseForm->IsWeapon() && baseForm->As<RE::TESObjectWEAP>()->formEnchanting) ||
		(baseForm && baseForm->IsArmor() && baseForm->As<RE::TESObjectARMO>()->formEnchanting) ||
		objectData->HasType(RE::ExtraDataType::kEnchantment)
	);
}

bool FoundEquipData::IsBroken() {
    if (!baseForm || !objectData) return false;

    // Get the text data
    RE::ExtraTextDisplayData* textData = objectData->GetExtraTextDisplayData();
	if (!textData) return false;

    std::string name = textData->displayName.c_str();
    const std::string brokenTag = "[B]";
    size_t pos = name.find(brokenTag);
	if (pos != std::string::npos)
		return true;
	else
		return false;
}

bool FoundEquipData::IsUnarmed() {
	return (baseForm && baseForm->formID == 0x0001F4);
}

bool FoundEquipData::CanBreak() {
    if (!baseForm || !objectData) return false;

    if (Settings::GetSingleton()->ED_BreakDisabled) return false;

    auto utility = Utility::GetSingleton();
	bool disallowMagic = Settings::GetSingleton()->ED_NoBreakNoEnchant &&
                         ((baseForm->IsWeapon() && baseForm->As<RE::TESObjectWEAP>()->HasKeyword(utility->keywordMagicDisallow)) ||
                          (baseForm->IsArmor()  && baseForm->As<RE::TESObjectARMO>()->HasKeyword(utility->keywordMagicDisallow)));

    return !disallowMagic &&
           !IsUnarmed() &&
           !Settings::GetSingleton()->HasNoBreakForms(baseForm->formID) &&
           !RE::InventoryEntryData(baseForm->As<RE::TESBoundObject>(), 1).IsQuestObject();
}

bool FoundEquipData::CanTemper() {
	if (!baseForm) return false;
	auto utility = Utility::GetSingleton();

    // Ignore Unarmed
    if (IsUnarmed()) return false;

	// Weapon processing
	if (auto* weap = baseForm->As<RE::TESObjectWEAP>())
		return !weap->IsStaff() && !weap->IsBound();

	// Armor processing
	else if (auto* armo = baseForm->As<RE::TESObjectARMO>()) {
		auto hasValidSlot = [&](auto... slots){ return (... || armo->HasPartOf(slots)); };
        return hasValidSlot(
                    RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kHair,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kShield
               ) &&
               !armo->HasKeyword(utility->keywordClothing) &&
               !armo->IsClothing();
	}

	return false;
}

// Private Functions
int FoundEquipData::GetEnchantmentListSize() {
    if (!baseForm) return 0;

    std::string type = GetType();
    if (type == "none") return 0;

    return Settings::GetSingleton()->GetEnchantmentSize(type);
}

std::string FoundEquipData::GetType() {
	if (baseForm->IsWeapon()) return "weapon";

	else if (auto* armor = baseForm->As<RE::TESObjectARMO>()) {
		auto hasSlot = [&](auto... slots){ return (... || armor->HasPartOf(slots)); };

        if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kTail)) return "body";

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kHair,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair)) return "head";

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kForearms)) return "hand";

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
                    RE::BGSBipedObjectForm::BipedObjectSlot::kCalves)) return "foot";

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) return "shield";
	}

	return "none";
}

int FoundEquipData::GetRandom(int a, int b) {
    thread_local std::mt19937 mt{ std::random_device{}() };  // one RNG per thread
    std::uniform_int_distribution<int> dist(a, b);
    return dist(mt);
}

float FoundEquipData::TruncateToDecimals(float value, int decimals) {
    float factor = std::pow(10.0f, decimals);
    float scaled = value * factor;
    float epsilon = 1e-6f;
    float truncated = std::trunc(scaled + (scaled >= 0 ? epsilon : -epsilon));
    return truncated / factor;
}

float FoundEquipData::RoundTo5Decimals(float value) {
    return std::round(value * 100000.0f) / 100000.0f;
}

// Static Functions
FoundEquipData FindEquippedWeapon(RE::InventoryChanges* a_Changes, RE::TESForm* a_Form, bool a_LeftHand) {
	FoundEquipData equipData(Utility::GetSingleton()->Unarmed);

	if (!a_Changes || !a_Changes->entryList)
		return equipData;

	for (const auto& entry : *a_Changes->entryList) {
		if (!entry || entry->GetObject() != a_Form || !entry->extraLists)
			continue;

		auto extraIt = std::find_if(entry->extraLists->begin(), entry->extraLists->end(), [&](const RE::ExtraDataList* pExtra) {
			return pExtra && pExtra->HasType(a_LeftHand ? RE::ExtraDataType::kWornLeft : RE::ExtraDataType::kWorn);
		});

		if (extraIt != entry->extraLists->end()) {
			equipData.baseForm = entry->GetObject();
            equipData.refForm = entry->GetObject();
			equipData.objectData = *extraIt;
			break;  // found first match, done
		}
	}

	return equipData;
}

FoundEquipData FindEquippedArmor(RE::InventoryChanges* a_Changes, RE::BGSBipedObjectForm::BipedObjectSlot a_SlotMask) {
    FoundEquipData equipData;

    if (!a_Changes || !a_Changes->entryList) return equipData;

    for (const auto& entry : *a_Changes->entryList) {
        if (!entry || !entry->extraLists) continue;

        auto* armor = entry->GetObject()->As<RE::TESObjectARMO>();
        if (!armor || !armor->HasPartOf(a_SlotMask)) continue;

		auto extraIt = std::find_if(entry->extraLists->begin(), entry->extraLists->end(), [](const RE::ExtraDataList* extra) {
			return extra && (extra->HasType(RE::ExtraDataType::kWorn) || extra->HasType(RE::ExtraDataType::kWornLeft));
		});

        if (extraIt != entry->extraLists->end()) {
            equipData.baseForm = armor;
            equipData.refForm = entry->GetObject();
            equipData.objectData = *extraIt;
            break; // found first valid equipped armor
        }
	}

    return equipData;
}