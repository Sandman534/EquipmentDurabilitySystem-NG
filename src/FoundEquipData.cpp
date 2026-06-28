#undef GetObject
#include "FoundEquipData.h"
#include "Utility.h"
#include "Settings.h"

// ===========================
// Item Name Functions
// ===========================
#pragma region Item Name Functions
void FoundEquipData::CreateName() {
    // Specifically create unarmed
	if (IsUnarmed()) {
		objectName = "Unarmed";
		return;
	}

    // Process normally
	if (!baseForm || !objectData)
        return;

    // Resolve the extra data to an object to pull information about it
    RE::InventoryEntryData entry(baseForm->As<RE::TESBoundObject>(), objectData->GetCount());
    entry.AddExtraList(objectData);

	// Get the basic name
    objectName = entry.GetDisplayName();

    // Show the poison attached the weapon
    auto settings = Settings::GetSingleton();
    const bool shouldShowPoison =
        baseForm->IsWeapon() &&
        settings->ED_Widget_ShowPoisonName &&
        objectData->HasType(RE::ExtraDataType::kPoison);

    if (!shouldShowPoison)
        return;

    auto* poisonData = static_cast<RE::ExtraPoison*>(objectData->GetByType(RE::ExtraDataType::kPoison));
    if (poisonData && poisonData->poison)
        objectName += " " + std::string(poisonData->poison->GetFullName()) + " (" + std::to_string(poisonData->count) + ")";
}

// ===========================
// Item Health Functions
// ===========================
float FoundEquipData::GetItemHealthForWidget() {
	if (!objectData || IsUnarmed())
        return 0.0f;

	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) 
        return Degredation::TruncateToDecimals(xHealth->health,3) + 0.001f;

	return Degredation::kDisplayHealth;
}

float FoundEquipData::GetItemHealthPercent() {
	if (!objectData)
        return Degredation::kMaxHealth;

    if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>())
        return xHealth->health;

	return Degredation::kMaxHealth;
}

float FoundEquipData::GetItemHealthRounded() {
    if (!objectData)
        return Degredation::kMaxHealth;

	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>())
        return Degredation::RoundToPrecision(xHealth->health);

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

    // Make sure to cap maximum health to 1.6. (Legendary)
    extraHealth->health = Degredation::RoundToPrecision((std::min)(value, 1.6999f));
}

// ===========================
// Enchantment Functions
// ===========================
void FoundEquipData::SetItemEnchantment(int playerLevel, RE::TESObjectREFR* ref)
{
	if (!baseForm)
		return;

	// --- Step 1: Determine player's max tier access ---
	int playerTier = 0;
	for (const auto& tier : GameData::TierTable) {
		if (playerLevel >= tier.minLevel) {
			playerTier = (std::max)(playerTier, tier.tier);
		}
	}
	if (playerTier == 0)
		return;

	// --- Step 2: Get material limits ---
	GameData::Material itemMaterial = getStrongestMaterial();
	auto it = GameData::MaterialTable.find(itemMaterial);
	if (it == GameData::MaterialTable.end())
		return;  // unknown material
	int materialMin = it->second.minTier;
	int materialMax = it->second.maxTier;

	// Player not high enough level for the enchantment
	if (playerTier < materialMin)
		return;

	// --- Step 3: Get Enchantment Vector based on Body Part ---]]
	std::vector<GameData::Enchantment>* allEnchantments = Settings::GetSingleton()->GetEnchantmentList(GetEquipmentType());
	std::vector<GameData::Enchantment> validEnchantments;

    // If we dont have enchantments
    if (!allEnchantments)
        return;

	// --- Step 4: Get Enchantment Vector based on Body Part ---
	int effectiveMinTier = materialMin;
	int effectiveMaxTier = (std::min)(playerTier, materialMax);
	std::copy_if(allEnchantments->begin(), allEnchantments->end(),
		std::back_inserter(validEnchantments), [effectiveMinTier, effectiveMaxTier](const GameData::Enchantment& e) {
			return e.tier >= effectiveMinTier && e.tier <= effectiveMaxTier;
		});

    // --- Step 4.5: Check if we have valid enchantments ---
    if (validEnchantments.empty())
        return;

	// --- Step 5: Random pick ---
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, static_cast<int>(validEnchantments.size()) - 1);
	GameData::Enchantment& chosen = validEnchantments[dist(gen)];

	// --- Step 6: Get charges from TierTable ---
	int chargeValue = 1;
	if (baseForm->IsWeapon()) {
		for (const auto& tier : GameData::TierTable) {
			if (tier.tier == chosen.tier) {
				chargeValue = tier.charge;
				break;
			}
		}
	}

	// --- Step 7: Apply the enchantment to the object ---
	if (!objectData->GetByType(RE::ExtraDataType::kEnchantment)) {
        // Set the objects enchantment
        auto* objectEnchantment = static_cast<RE::ExtraEnchantment*>(RE::ExtraEnchantment::Create(sizeof(RE::ExtraEnchantment), RE::VTABLE_ExtraEnchantment[0].address()));
		objectEnchantment->enchantment = chosen.enchantment;
		objectEnchantment->charge = chargeValue;
		objectData->Add(objectEnchantment);

        // If the weapon is being held by an actor, set a charge value
		if (RE::Actor* a_actor = ref->As<RE::Actor>(); a_actor && baseForm->IsWeapon()) {
			int RandomEnchant = Random::Int(0,chargeValue);
			if (objectData->HasType<RE::ExtraWornLeft>())
                a_actor->AsActorValueOwner()->ModActorValue(RE::ActorValue::kLeftItemCharge, RandomEnchant);
			else if (objectData->HasType<RE::ExtraWorn>())
                a_actor->AsActorValueOwner()->ModActorValue(RE::ActorValue::kRightItemCharge, RandomEnchant);
		}

        // --- Step 8: Rename the object ---
        objectName = std::string(baseForm->GetName()) + " " + chosen.suffix;
        auto* xTextData = static_cast<RE::ExtraTextDisplayData*>(objectData->GetByType(RE::ExtraDataType::kTextDisplayData));
        if (!xTextData) {
			xTextData = static_cast<RE::ExtraTextDisplayData*>(RE::ExtraTextDisplayData::Create(sizeof(RE::ExtraTextDisplayData), RE::VTABLE_ExtraTextDisplayData[0].address()));
            objectData->Add(xTextData);
        }
        xTextData->SetName(objectName.c_str());
	}
}

// ===========================
// Item Process Functions
// ===========================
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
    extraHealth->health = Degredation::RoundToPrecision(Degredation::kMaxHealth);
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
	return objectData && (
		(baseForm && baseForm->IsWeapon() && baseForm->As<RE::TESObjectWEAP>()->formEnchanting) ||
		(baseForm && baseForm->IsArmor() && baseForm->As<RE::TESObjectARMO>()->formEnchanting) ||
		objectData->HasType(RE::ExtraDataType::kEnchantment)
	);
}

bool FoundEquipData::IsBroken() {
    if (!baseForm || !objectData) return false;
	if (GetItemHealthPercent() < Degredation::kBrokenHealth) return true;
    return false;
}

bool FoundEquipData::IsUnarmed() {
	return (baseForm && baseForm->formID == 0x0001F4);
}

bool FoundEquipData::CanBreak() {
	// Run various checks to see if the item is breakable
    if (!baseForm || !objectData)
        return false;

    // Already broken or unarmed
    auto settings = Settings::GetSingleton();
    if (settings->ED_BreakDisabled || IsBroken() || IsUnarmed())
        return false;

    // No break form list, or is a quest item
    if (settings->HasNoBreakForms(baseForm->formID) || objectData->HasQuestObjectAlias())
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

bool FoundEquipData::CanTemper() {
    //No form or unarmed
	if (!baseForm || IsUnarmed()) return false;

	// Weapon processing
	if (auto* weap = baseForm->As<RE::TESObjectWEAP>())
		return !weap->IsStaff() && !weap->IsBound();

	// Armor processing
	if (auto* armo = baseForm->As<RE::TESObjectARMO>()) {
        const bool hasValidSlot =
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHead) ||
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHair) ||
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kBody) ||
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kHands) ||
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet) ||
            armo->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield);

        return hasValidSlot &&
            armo->GetPlayable() &&
            !armo->HasKeyword(Utility::GetSingleton()->keywordClothing) &&
            !armo->IsClothing() &&
            armo->GetPlayable();
	}

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
    for (int i = GameData::TierTable.size() - 1; i >= 0; --i) {
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
