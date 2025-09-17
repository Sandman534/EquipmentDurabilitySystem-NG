#undef GetObject
#include "FoundEquipData.h"
#include "Utility.h"
#include "Settings.h"

// ===========================
// Main Functions
// ===========================
#pragma region Item Name Functions
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
#pragma endregion

#pragma region Item Health Functions
float FoundEquipData::GetItemHealthForWidget() {
	if (IsUnarmed()) return 0.0f;
	if (!objectData) return 1.1f;
	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return TruncateToDecimals(xHealth->health,3) + 0.001f;
	return 1.1f;
}

float FoundEquipData::GetItemHealthPercent() {
	if (!objectData) return 1.099f;
    if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return xHealth->health;
	return 1.099f;
}

float FoundEquipData::GetItemHealthRounded() {
    if (!objectData) return 1.099f;
	if (auto* xHealth = objectData->GetByType<RE::ExtraHealth>()) return RoundTo5Decimals(xHealth->health);
	return 1.099f;
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
#pragma endregion

#pragma region Item Enchantment Functions
void FoundEquipData::SetItemEnchantment(int playerLevel, RE::TESObjectREFR* ref)
{
	if (!baseForm)
		return;

	// --- Step 1: Determine player’s max tier access ---
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

	// --- Step 3: Get Enchantment Vector based on Body Part ---
	std::vector<GameData::Enchantment>* allEnchantments = Settings::GetSingleton()->GetEnchantmentList(GetEquipmentType());
	std::vector<GameData::Enchantment> validEnchantments;

	// --- Step 4: Get Enchantment Vector based on Body Part ---
	int effectiveMinTier = materialMin;
	int effectiveMaxTier = (std::min)(playerTier, materialMax);
	std::copy_if(allEnchantments->begin(), allEnchantments->end(),
		std::back_inserter(validEnchantments), [effectiveMinTier, effectiveMaxTier](const GameData::Enchantment& e) {
			return e.tier >= effectiveMinTier && e.tier <= effectiveMaxTier;
		});

	// --- Step 4: Random pick ---
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, static_cast<int>(validEnchantments.size()) - 1);
	GameData::Enchantment& chosen = validEnchantments[dist(gen)];

	// --- Step 5: Get charges from TierTable ---
	int chargeValue = 1;
	if (baseForm->IsWeapon()) {
		for (const auto& tier : GameData::TierTable) {
			if (tier.tier == chosen.tier) {
				chargeValue = tier.charge;
				break;
			}
		}
	}

	// --- Step 6: Apply the enchantment to the object ---
	if (!objectData->GetByType(RE::ExtraDataType::kEnchantment)) {
        // Set the objects enchantment
        auto* objectEnchantment = static_cast<RE::ExtraEnchantment*>(RE::ExtraEnchantment::Create(sizeof(RE::ExtraEnchantment), RE::VTABLE_ExtraEnchantment[0].address()));
		objectEnchantment->enchantment = chosen.enchantment;
		objectEnchantment->charge = chargeValue;
		objectData->Add(objectEnchantment);

        // If the weapon is being held by an actor, set a charge value
		if (RE::Actor* a_actor = ref->As<RE::Actor>(); a_actor && baseForm->IsWeapon()) {
			int RandomEnchant = GetRandom(0,chargeValue);
			if (objectData->HasType<RE::ExtraWornLeft>())
				a_actor->AsActorValueOwner()->ModActorValue(RE::ActorValue::kLeftItemCharge, RandomEnchant);
			else if (objectData->HasType<RE::ExtraWorn>())
                a_actor->AsActorValueOwner()->ModActorValue(RE::ActorValue::kRightItemCharge, RandomEnchant);
		}

        // --- Step 7: Rename the object ---
        objectName = std::string(baseForm->GetName()) + " " + chosen.suffix;
        auto* xTextData = static_cast<RE::ExtraTextDisplayData*>(objectData->GetByType(RE::ExtraDataType::kTextDisplayData));
        if (!xTextData) {
			xTextData = static_cast<RE::ExtraTextDisplayData*>(RE::ExtraTextDisplayData::Create(sizeof(RE::ExtraTextDisplayData), RE::VTABLE_ExtraTextDisplayData[0].address()));
            objectData->Add(xTextData);
        }
        xTextData->SetName(objectName.c_str());
	}
}
#pragma endregion

#pragma region Item Process Functions
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
    extraHealth->health = RoundTo5Decimals(1.099f);
}
#pragma endregion

#pragma region Item Test Functions
bool FoundEquipData::IsTempered() {
	return objectData 
        && objectData->GetByType<RE::ExtraHealth>() != nullptr 
        && objectData->GetByType<RE::ExtraHealth>()->health > 1.099f;
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
	if (GetItemHealthPercent() < 0.5f) return true;
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
#pragma endregion

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

std::string FoundEquipData::GetEquipmentType() {
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