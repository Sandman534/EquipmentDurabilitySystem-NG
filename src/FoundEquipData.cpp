#undef GetObject
#include "FoundEquipData.h"
#include "Utility.h"
#include "Settings.h"

// ===========================
// Item Name Functions
// ===========================
void FoundEquipData::CreateName() {
    // Specifically create unarmed
	if (IsUnarmed()) {
		objectName = "Unarmed";
		return;
	}

    // Process normally
	if (!baseForm || !objectData)
        return;

	// Get the basic name
    objectName = objectData->GetDisplayName(baseForm->As<RE::TESBoundObject>());

    // Show the poison attached the weapon
    auto settings = Settings::GetSingleton();
    if (!baseForm->IsWeapon() || !settings->ED_Widget_ShowPoisonName || !objectData->HasType(RE::ExtraDataType::kPoison))
        return;

    auto* poisonData = static_cast<RE::ExtraPoison*>(objectData->GetByType(RE::ExtraDataType::kPoison));
    if (poisonData && poisonData->poison)
        objectName += " " + std::string(poisonData->poison->GetFullName()) + " (" + std::to_string(poisonData->count) + ")";
}

// ===========================
// Item Health Functions
// ===========================
float FoundEquipData::GetItemHealthForWidget() {
    auto objectHealth = GetItemHealthPercent();
    if (objectHealth < Degredation::kBrokenHealth) return 0.0f;
    return static_cast<float>(std::max(std::round((Degredation::TruncateToDecimals(objectHealth,3) - Degredation::kMinHealth) * 1000.0), 0.0));
}

float FoundEquipData::GetItemHealthPercent() {
    // If no object data, return standard health
	if (!objectData) return Degredation::kMaxHealth;

    // If there is extra health and its not 0
    if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>())
        if (xHealth->health != 0.0f)
            if (xHealth->health < Degredation::kBrokenHealthThreshold)
                return Degredation::kBrokenHealth;
            else
                return xHealth->health;

    // Otherwise always return standard health
	return Degredation::kMaxHealth;
}

void FoundEquipData::SetItemHealthPercent(float value) {
	if (!objectData || IsUnarmed())
        return;

    auto* extraHealth = objectData->GetByType<RE::ExtraHealth>();
    if (!extraHealth) {
        extraHealth = static_cast<RE::ExtraHealth*>(
            RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address())
        );
        objectData->Add(extraHealth);
    }

    // Set the value of health
    extraHealth->health = Degredation::RoundToPrecision(value);
}

void FoundEquipData::SetItemHealthPercentCapped(float value) {
	if (!objectData || IsUnarmed())
        return;

    auto* extraHealth = objectData->GetByType<RE::ExtraHealth>();
    if (!extraHealth) {
        extraHealth = static_cast<RE::ExtraHealth*>(
            RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address())
        );
        objectData->Add(extraHealth);
    }

    // Make sure to cap maximum health to 1.6. (Legendary)
    extraHealth->health = Degredation::RoundToPrecision((std::min)(value, 1.6000f));
}

// ===========================
// Enchantment Functions
// ===========================
void FoundEquipData::SetItemEnchantment(int playerLevel, RE::TESObjectREFR* ref) {
	if (!baseForm) return;

	// --- Step 1: Determine player's max tier access ---
	int playerTier = 0;
	for (const auto& tier : GameData::TierTable) {
		if (playerLevel >= tier.minLevel)
			playerTier = (std::max)(playerTier, tier.tier);
	}
	if (playerTier == 0) return;

	// --- Step 2: Get material limits ---
	GameData::Material itemMaterial = getStrongestMaterial();
	auto it = GameData::MaterialTable.find(itemMaterial);
	if (it == GameData::MaterialTable.end()) return;  // unknown material
	int materialMin = it->second.minTier;
	int materialMax = it->second.maxTier;

	// Player not high enough level for the enchantment
	if (playerTier < materialMin) return;

	// --- Step 3: Get Enchantment Vector based on Body Part ---
	std::vector<GameData::Enchantment>* allEnchantments = Settings::GetSingleton()->GetEnchantmentList(GetEquipmentType());
	std::vector<GameData::Enchantment> validEnchantments;

    // If we dont have enchantments
    if (!allEnchantments) return;

	// --- Step 4: Get Enchantment Vector based on Body Part ---
	int effectiveMinTier = materialMin;
	int effectiveMaxTier = (std::min)(playerTier, materialMax);
	std::copy_if(allEnchantments->begin(), allEnchantments->end(),
		std::back_inserter(validEnchantments), [effectiveMinTier, effectiveMaxTier](const GameData::Enchantment& e) {
			return e.tier >= effectiveMinTier && e.tier <= effectiveMaxTier;
		});

    // --- Step 4.5: Check if we have valid enchantments ---
    if (validEnchantments.empty()) return;

	// --- Step 5: Random pick ---
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, static_cast<int>(validEnchantments.size()) - 1);
	GameData::Enchantment& chosen = validEnchantments[dist(gen)];

	// --- Step 6: Get charges from TierTable ---
	std::uint16_t chargeValue = 1;
	if (baseForm->IsWeapon()) {
		for (const auto& tier : GameData::TierTable) {
			if (tier.tier == chosen.tier) {
				chargeValue = static_cast<std::uint16_t>(tier.charge);
				break;
			}
		}
	}

	// --- Step 7: Apply the enchantment to the object ---
	if (objectData->HasType<RE::ExtraEnchantment>()) return;
    objectData->SetEnchantment(chosen.enchantment, chargeValue, false);

    // --- Step 8: Set Initial Charge ---
    auto* extraCharge = objectData->GetByType<RE::ExtraCharge>();
    if (!extraCharge) {
        extraCharge = RE::BSExtraData::Create<RE::ExtraCharge>();
        if (!extraCharge) return;
        extraCharge->charge = static_cast<float>(Random::Int(1, static_cast<int>(chargeValue)));
        objectData->Add(extraCharge);
    } else
        extraCharge->charge = static_cast<float>(Random::Int(1, static_cast<int>(chargeValue)));

    // --- Step 9: Rename the object ---
    objectName = std::string(baseForm->GetName()) + " " + chosen.suffix;
    auto* xTextData = static_cast<RE::ExtraTextDisplayData*>(objectData->GetByType(RE::ExtraDataType::kTextDisplayData));
    if (!xTextData) {
        xTextData = static_cast<RE::ExtraTextDisplayData*>(RE::ExtraTextDisplayData::Create(sizeof(RE::ExtraTextDisplayData), RE::VTABLE_ExtraTextDisplayData[0].address()));
        objectData->Add(xTextData);
    }
    xTextData->SetName(objectName.c_str());

    // --- Step 10: Set charge level of equipped object ---
    if (auto* actor = ref ? ref->As<RE::Actor>() : nullptr) {
        auto* actorValueOwner = actor->AsActorValueOwner();
        if (actorValueOwner) {
            if (objectData->HasType<RE::ExtraWorn>())
                actorValueOwner->SetActorValue(RE::ActorValue::kRightItemCharge, extraCharge->charge);

            if (objectData->HasType<RE::ExtraWornLeft>())
                actorValueOwner->SetActorValue(RE::ActorValue::kLeftItemCharge, extraCharge->charge);
        }
    }
}

const char* FoundEquipData::GetItemName() {
    auto* xTextName = baseForm->GetName();
    if (auto* xTextData = static_cast<RE::ExtraTextDisplayData*>(objectData->GetByType(RE::ExtraDataType::kTextDisplayData)))
        xTextName = xTextData->displayName.c_str();

    return xTextName;
}

// ===========================
// Item Process Functions
// ===========================
bool FoundEquipData::CanProcessData() {
    if (!baseForm || !objectData || IsUnarmed()) return false;

    // Already has ExtraHealth
    if (auto* ExtraHealth = objectData->GetByType<RE::ExtraHealth>())
        if (ExtraHealth) return false;

    // Is a quest item
    if (objectData->HasQuestObjectAlias()) return false;

    return true;
}

bool FoundEquipData::CanProcessObject() {
    if (!baseForm || IsUnarmed()) return false;

    // Is a Weapon
    if (auto* weapon = baseForm->As<RE::TESObjectWEAP>()) {
        if (weapon->IsBound() || weapon->IsStaff()) return false;
        return true;
    }

    // Is Armor
    if (auto* armor = baseForm->As<RE::TESObjectARMO>()) 
        return true;

    return false;
}

void FoundEquipData::ProcessItem() {
    if (!objectData) return;
    auto* ExtraHealth = objectData->GetByType<RE::ExtraHealth>();
    if (!ExtraHealth) {
        ExtraHealth = static_cast<RE::ExtraHealth*>(
            RE::ExtraHealth::Create(sizeof(RE::ExtraHealth), RE::VTABLE_ExtraHealth[0].address())
        );
        objectData->Add(ExtraHealth);
    }
}

// ===========================
// Item Test Functions
// ===========================
bool FoundEquipData::IsTempered() {
    if (!objectData) return false;
    auto* xHealth = objectData->GetByType<RE::ExtraHealth>();
    return xHealth && xHealth->health > Degredation::kMaxHealth;
}

bool FoundEquipData::IsEnchanted() {
    if (!baseForm || !objectData) return false;
	return objectData->HasType(RE::ExtraDataType::kEnchantment) ||
        (baseForm->IsWeapon() && baseForm->As<RE::TESObjectWEAP>()->formEnchanting) ||
		(baseForm->IsArmor() && baseForm->As<RE::TESObjectARMO>()->formEnchanting);
}

bool FoundEquipData::IsBroken() {
    if (!baseForm || !objectData) return false;
	if (GetItemHealthPercent() < Degredation::kBrokenHealthThreshold) return true;
    return false;
}

bool FoundEquipData::IsUnarmed() {
    if (!baseForm) return false;
	return baseForm->formID == 0x0001F4;
}

bool FoundEquipData::CanTemper() {
    if (!baseForm || IsUnarmed()) return false;

    // Do not effect Non-Playable items
    if (!baseForm->GetPlayable())
        return false;

    // Look for Template Weapon
    if (auto* weapon = baseForm->As<RE::TESObjectWEAP>())
        if (auto* tempweap = weapon->templateWeapon)
            return Utility::GetSingleton()->TemperableForms.contains(tempweap->formID);

    // Look for Template Armor
    if (auto* armor = baseForm->As<RE::TESObjectARMO>())
        if (auto* temparmo = armor->templateArmor)
            return Utility::GetSingleton()->TemperableForms.contains(temparmo->formID);

    // Look for the base object
    return Utility::GetSingleton()->TemperableForms.contains(baseForm->formID);
}

bool FoundEquipData::CanBreak() {
	// Run various checks to see if the item is breakable
    if (!baseForm || !objectData)
        return false;

    // Already broken or unarmed
    auto settings = Settings::GetSingleton();
    if (settings->ED_BreakDisabled || IsBroken() || IsUnarmed())
        return false;

    // Non-Playable or Quest Items
    if (!baseForm->GetPlayable() || objectData->HasQuestObjectAlias())
        return false;

    // Determine if the object is breakable based on other factors
    auto utility = Utility::GetSingleton();
    if (settings->ED_NoBreakNoEnchant) {
        if (auto* weapon = baseForm->As<RE::TESObjectWEAP>())
            return !weapon->HasKeyword(utility->keywordMagicDisallow);

        if (auto* armor = baseForm->As<RE::TESObjectARMO>())
            return !armor->HasKeyword(utility->keywordMagicDisallow);
    }

    return true;
}

bool FoundEquipData::CanEnchant() {
    if (!baseForm) return false;
    auto* foundKeyword = Utility::GetSingleton()->keywordMagicDisallow;

    // Do not effect Non-Playable items
    if (!baseForm->GetPlayable())
        return false;

    // Check for Weapon
    if (auto* weapon = baseForm->As<RE::TESObjectWEAP>())
        return !weapon->HasKeyword(foundKeyword);

    // Check for Armor
    if (auto* armor = baseForm->As<RE::TESObjectARMO>())
        return !armor->HasKeyword(foundKeyword);

    return false;
}

bool FoundEquipData::IsBelowBreakingThreshold() {
    if (!CanBreak() || !CanTemper()) return false;

    // Check for Weapon
    if (GetItemHealthForWidget() <= Settings::GetSingleton()->ED_BreakThreshold)
        return true;

    return false;
}

// ===========================
// Private Functions
// ===========================
std::optional<GameData::TierInfo> FoundEquipData::GetTierForLevel(GameData::Material mat, int playerLevel) {
    // Find material limits
    auto itMat = GameData::MaterialTable.find(mat);
    if (itMat == GameData::MaterialTable.end())
        return std::nullopt; // material not found

    const auto& limits = itMat->second;

    // Search tiers from highest to lowest
    for (auto i = GameData::TierTable.size() - 1; i >= 0; --i) {
        const GameData::TierInfo& tier = GameData::TierTable[i];

        // Tier must be within material bounds
        if (tier.tier < limits.minTier || tier.tier > limits.maxTier)
            continue;

        // Player level must meet tier's minimum
        if (playerLevel >= tier.minLevel)
            return tier;
    }

    // Player too low for this material
    return std::nullopt;
}

GameData::Material FoundEquipData::getStrongestMaterial() {
    GameData::Material bestMaterial = GameData::Material::Fur; // placeholder

    // Material Lists
    std::vector<GameData::Material> itemMaterials;
    std::unordered_set<GameData::Material> seenMaterials; // avoid duplicates

    // Go through all keywords and find the matching materials
    RE::BGSKeywordForm* keywordList = baseForm->As<RE::BGSKeywordForm>();
    if (!keywordList) return bestMaterial; // safety check
    for (auto* keyword : keywordList->GetKeywords()) {
        if (!keyword) continue;

        auto it = Settings::GetSingleton()->materialMap.find(keyword->formID);
        if (it != Settings::GetSingleton()->materialMap.end()) {
            GameData::Material mat = it->second;

            // Avoid duplicates
            if (seenMaterials.find(mat) == seenMaterials.end()) {
                itemMaterials.push_back(mat);
                seenMaterials.insert(mat);
            }
        }
    }

    // Get the Tier of the best material on the object
    int bestMinTier = 0;
    int bestMaxTier = 0;

    for (GameData::Material mat : itemMaterials) {
        auto it = GameData::MaterialTable.find(mat);
        if (it != GameData::MaterialTable.end()) {
            const GameData::MaterialLimits& limits = it->second;

            // Choose the material with highest minTier first, then highest maxTier
            if (limits.minTier > bestMinTier || (limits.minTier == bestMinTier && limits.maxTier > bestMaxTier)) {
                bestMaterial = mat;
                bestMinTier = limits.minTier;
                bestMaxTier = limits.maxTier;
            }
        }
    }

    return bestMaterial;
}

EquipmentType FoundEquipData::GetEquipmentType() {
	if (baseForm->IsWeapon())
        return EquipmentType::Weapon;

	else if (auto* armor = baseForm->As<RE::TESObjectARMO>()) {
		auto hasSlot = [&](auto... slots){ return (... || armor->HasPartOf(slots)); };

        if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
                RE::BGSBipedObjectForm::BipedObjectSlot::kTail))
            return EquipmentType::Body;

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
                RE::BGSBipedObjectForm::BipedObjectSlot::kHair,
                RE::BGSBipedObjectForm::BipedObjectSlot::kLongHair)) 
            return EquipmentType::Head;

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
                RE::BGSBipedObjectForm::BipedObjectSlot::kForearms))
            return EquipmentType::Hand;

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
                RE::BGSBipedObjectForm::BipedObjectSlot::kCalves))
            return EquipmentType::Foot;

        else if (hasSlot(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) 
            return EquipmentType::Shield;
	}

	return EquipmentType::None;
}

// ===========================
// Static FoundEquipData
// ===========================
FoundEquipData FindEquippedWeapon(RE::InventoryChanges* a_Changes, RE::Actor* a_actor, RE::TESForm* a_Form, bool a_LeftHand) {
	FoundEquipData equipData(Utility::GetSingleton()->Unarmed);

    // Get Inventory Records
    if (!a_Changes || !a_Changes->entryList || !a_actor || !a_Form) return equipData;

    // Get Both equipped hands
    auto* rightForm  = a_actor->GetEquippedObject(false);
    auto* leftForm = a_actor->GetEquippedObject(true);

    // Setup variables to search forms
    bool matching_forms = rightForm == leftForm;
    auto wornType = a_LeftHand ? RE::ExtraDataType::kWornLeft : RE::ExtraDataType::kWorn;

    // Determine which weapon record to use
	for (const auto& entry : *a_Changes->entryList) {
		if (!entry || entry->GetObject() != a_Form || !entry->extraLists)
			continue;

        // Identical base forms require the requested hand's worn marker. If
        // the forms differ, accept either marker on the uniquely matched form.
		auto extraIt = std::find_if(entry->extraLists->begin(), entry->extraLists->end(), [&](const RE::ExtraDataList* pExtra) {
			if (!pExtra) return false;

			if (matching_forms)
                return pExtra->HasType(wornType);

			return pExtra->HasType(RE::ExtraDataType::kWornLeft) || pExtra->HasType(RE::ExtraDataType::kWorn);
		});

        // Set the data
		if (extraIt != entry->extraLists->end()) {
			equipData.baseForm = entry->GetObject();
            equipData.refForm = entry->GetObject();
			equipData.objectData = *extraIt;
			return equipData;
		}
	}

	return equipData;
}

FoundEquipData FindEquippedArmor(RE::InventoryChanges* a_Changes, RE::BGSBipedObjectForm::BipedObjectSlot a_SlotMask) {
    FoundEquipData equipData;

    // Check Inventory Records
    if (!a_Changes || !a_Changes->entryList) return equipData;

    // Determine which of the armor records to use
    for (const auto& entry : *a_Changes->entryList) {
        if (!entry || !entry->extraLists) continue;

        auto* armor = entry->GetObject()->As<RE::TESObjectARMO>();
        if (!armor || !armor->HasPartOf(a_SlotMask)) continue;

		auto extraIt = std::find_if(entry->extraLists->begin(), entry->extraLists->end(), [](const RE::ExtraDataList* extra) {
			return extra && (extra->HasType(RE::ExtraDataType::kWorn) || extra->HasType(RE::ExtraDataType::kWornLeft));
		});

        if (extraIt != entry->extraLists->end()) {
            equipData.baseForm = entry->GetObject();
            equipData.refForm = entry->GetObject();
            equipData.objectData = *extraIt;
            return equipData;
        }
	}

    return equipData;
}
