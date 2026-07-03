#undef GetObject
#include "Events.h"
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"
#include <unordered_set>
#include <set>
#include <array>

// =============================================================
// Actor Hit Throttle
// =============================================================
std::unordered_set<RE::Actor*> actorList;
std::mutex actorLock;

static bool AddActor(RE::Actor* actor) {
    std::lock_guard<std::mutex> guard(actorLock);
    return actorList.insert(actor).second;
}

static void ClearActor() {
    std::lock_guard<std::mutex> guard(actorLock);
    actorList.clear();
}

// =============================================================
// Temper / Decay Functions
// =============================================================
static void RemoveEquipment(FoundEquipData* eqD, RE::Actor* actor) {
	if (!actor || !eqD->baseForm) return;
	auto utility = Utility::GetSingleton();

    // Player notification
    if (actor == utility->GetPlayer()) {
        eqD->CreateName();
        auto msg = std::format("Your {} has broken", eqD->objectName);
        utility->ShowNotification(msg, false, "VOCShoutImpactDisarm");
    }

	// Unequip and mark weapon as broken
	if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
		equipMgr->UnequipObject(actor, eqD->refForm, eqD->objectData, 1, nullptr, true, false, true, false, nullptr);

	// Update the name and set the health value to something lower than minimum
	eqD->SetItemHealthPercent(Degredation::kBrokenHealth);
}

static void TemperDecay(FoundEquipData* eqD, RE::Actor* actor, bool powerAttack) {
	auto utility = Utility::GetSingleton();
	auto setting = Settings::GetSingleton();

	if (setting->ED_DegradationDisabled) return;
	if (!eqD->baseForm || !AddActor(actor)) return;
	if (eqD->baseForm == utility->Unarmed) return;

	// Get current health percent
	float CurrentHealth = eqD->GetItemHealthPercent();
	float BreakThreshold = (setting->ED_BreakThreshold / 1000.0f);
	
	// --- Break Chance ---
	if ((CurrentHealth - Degredation::kMinHealth) <= BreakThreshold) {
		float chance = setting->GetBreakChance(eqD->baseForm, actor);

		// Apply modifiers
		if (chance != 0.0 && eqD->CanBreak()) {

			// Increased Durability
			if (setting->ED_IncreasedDurability && CurrentHealth > Degredation::kMinHealth) {
				double durabilityChance = 1.0 - ((CurrentHealth - Degredation::kMinHealth) / BreakThreshold);
				
				// A negative durability chance means it wont break at all, lets adjust that 
				if (durabilityChance <= 0)
					chance *= 0.01;
				else
					chance *= durabilityChance;
			}

			// Power Attack Multiplier
			if (powerAttack) 
				chance *= 1.0 + (setting->ED_Break_PowerAttack / 100.0);

			// Follower/NPC Multiplier
			if (actor != utility->GetPlayer())
				chance *= actor->IsPlayerTeammate()
					? 1.0 + (setting->ED_Break_FollowerMulti / 100.0)
					: 1.0 + (setting->ED_Break_NPCMulti / 100.0);

			// Check to see if we break
			if (Probability::Double(chance)) {
				RemoveEquipment(eqD, actor);
				return;
			}
		}
	}

	// --- Degradation ---
	if (CurrentHealth <= Degredation::kMinHealth) return;

	double degrade_rate = setting->GetDegradationRate(eqD->baseForm, actor);
	if (degrade_rate == 0) return;

	// Determine the health rate based on the defined curve
	double rate = std::clamp(degrade_rate, 0.0, 200.0);
	double scale = std::pow(rate / 100.0, Degredation::kCurve);
	double loss = Random::Double(Degredation::kMinLossAt100, Degredation::kMaxLossAt100) * scale;

	// Power Attack Multiplier
	if (powerAttack)
		loss *= 1.0 + (setting->ED_Degrade_PowerAttack / 100.0);

	// Follower/NPC Multiplier
	if (actor != utility->GetPlayer())
		loss *= actor->IsPlayerTeammate() 
			? 1.0 + (setting->ED_Degrade_FollowerMulti / 100.0)
			: 1.0 + (setting->ED_Degrade_NPCMulti / 100.0);

	// Apply the lost health
	CurrentHealth -= loss;
	CurrentHealth = std::round(CurrentHealth * Degredation::kPrecision) / Degredation::kPrecision;

	// The default health of an item is always one, so it cant go lower
	if (CurrentHealth < Degredation::kMinHealth)
		CurrentHealth = Degredation::kMinHealth;

	// Set the new health of the item
	eqD->SetItemHealthPercent(CurrentHealth);
}

// =============================================================
// On Hit: Decay Equipment
// =============================================================
static bool ShouldProcessActor(RE::Actor* actor) {
    if (!actor) return false;

    auto* settings = Settings::GetSingleton();
    auto* utility = Utility::GetSingleton();
    auto* player = utility->GetPlayer();

    if (!utility->ActorIsNotBeast(actor))
		return false;

    if (actor == player)
        return settings->ED_AffectPlayer;

    if (actor->IsPlayerTeammate())
        return settings->ED_AffectFollower;

    return settings->ED_AffectNPC;
}

static bool IsValidHitSource(RE::TESForm* form) {
    if (!form) return false;

    if (auto* weapon = form->As<RE::TESObjectWEAP>())
        return !weapon->IsStaff();

    if (auto* armor = form->As<RE::TESObjectARMO>())
        return armor->IsShield();

    return false;
}

static void ShuffleSlots(std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4>& slots) {
	thread_local std::mt19937 mt{ std::random_device{}() };
	std::shuffle(slots.begin(), slots.end(), mt);
}

static void DecayBlockingEquipment(RE::Actor* actor, RE::InventoryChanges* changes, bool powerAttack) {
	FoundEquipData shield = FindEquippedArmor(changes, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
	if (shield.baseForm) {
		TemperDecay(&shield, actor, powerAttack);
		return;
	}

	RE::TESForm* rightHand = actor->GetEquippedObject(false);
	if (!rightHand) return;

	auto* weapon = rightHand->As<RE::TESObjectWEAP>();
	if (!weapon || weapon->IsBound()) return;

	FoundEquipData weaponData = FindEquippedWeapon(changes, rightHand, false);
	TemperDecay(&weaponData, actor, powerAttack);
}

static void DecayRandomArmorPiece(RE::Actor* actor, RE::InventoryChanges* changes, bool powerAttack) {
	std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4> slots = {
		RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
		RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
		RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
		RE::BGSBipedObjectForm::BipedObjectSlot::kFeet
	};

	ShuffleSlots(slots);

	for (auto slot : slots) {
		FoundEquipData armor = FindEquippedArmor(changes, slot);
		if (armor.baseForm) {
			TemperDecay(&armor, actor, powerAttack);
			return;
		}

		if (slot != RE::BGSBipedObjectForm::BipedObjectSlot::kHead) continue;

		armor = FindEquippedArmor(changes, RE::BGSBipedObjectForm::BipedObjectSlot::kHair);
		if (armor.baseForm) {
			TemperDecay(&armor, actor, powerAttack);
			return;
		}
	}
}

static bool IsLeftHandAttack(RE::Actor* actor) {
	bool leftHandAttack = false;
	return actor->GetGraphVariableBool("bLeftHandAttack", leftHandAttack) && leftHandAttack;
}

static void DecayAttackingWeapon(RE::Actor* actor, RE::InventoryChanges* changes, RE::TESForm* form, bool powerAttack) {
	auto* weapon = form->As<RE::TESObjectWEAP>();
	if (!weapon || weapon->IsStaff() || weapon->IsBound()) return;

	const bool equippedRight = form == actor->GetEquippedObject(false);
	const bool equippedLeft = form == actor->GetEquippedObject(true);

	if (!equippedRight && !equippedLeft) return;

	const bool useLeftHand = equippedLeft && (!equippedRight || IsLeftHandAttack(actor));
	FoundEquipData weaponData = FindEquippedWeapon(changes, form, useLeftHand);
	TemperDecay(&weaponData, actor, powerAttack);
}

static void DecayAttackingShield(RE::Actor* actor, RE::InventoryChanges* changes, RE::TESForm* form, bool powerAttack) {
	auto* armor = form->As<RE::TESObjectARMO>();
	if (!armor || !armor->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) return;

	FoundEquipData shield = FindEquippedArmor(changes, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
	TemperDecay(&shield, actor, powerAttack);
}

static void ProcessDefenderHit(const RE::TESHitEvent* event, bool powerAttack) {
	if (!event->target || event->target->formType != RE::FormType::ActorCharacter) return;

	RE::Actor* actor = event->target->As<RE::Actor>();
	if (!ShouldProcessActor(actor)) return;

	RE::InventoryChanges* changes = actor->GetInventoryChanges();
	if (!changes) return;

	RE::TESForm* form = RE::TESForm::LookupByID(event->source);
	if (!IsValidHitSource(form)) return;

	if (event->flags.any(RE::TESHitEvent::Flag::kHitBlocked)) {
		DecayBlockingEquipment(actor, changes, powerAttack);
		return;
	}

	DecayRandomArmorPiece(actor, changes, powerAttack);
}

static void ProcessAttackerHit(const RE::TESHitEvent* event, bool powerAttack) {
	if (!event->cause || event->cause->formType != RE::FormType::ActorCharacter) return;

	RE::Actor* actor = event->cause->As<RE::Actor>();
	if (!ShouldProcessActor(actor)) return;

	RE::InventoryChanges* changes = actor->GetInventoryChanges();
	if (!changes) return;

	RE::TESForm* form = RE::TESForm::LookupByID(event->source);
	if (!form) return;

	if (form->IsWeapon()) {
		DecayAttackingWeapon(actor, changes, form, powerAttack);
		return;
	}

	if (form->IsArmor()) {
		DecayAttackingShield(actor, changes, form, powerAttack);
	}
}

class HitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {
public:
    static HitEventHandler* GetSingleton() {
        static HitEventHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override {
		if (!a_event) return RE::BSEventNotifyControl::kContinue;
		if (Settings::GetSingleton()->ED_DegradationDisabled) return RE::BSEventNotifyControl::kContinue;

		// Determine power attack
		const bool powerAttack =
			a_event->flags.any(RE::TESHitEvent::Flag::kPowerAttack) ||
			(a_event->cause &&
				a_event->cause->formType == RE::FormType::ActorCharacter &&
				a_event->cause->As<RE::Actor>()->IsPowerAttacking());

		ProcessDefenderHit(a_event, powerAttack);
		ProcessAttackerHit(a_event, powerAttack);

		return RE::BSEventNotifyControl::kContinue;
    }

    static void Register() {
        RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
        eventHolder->AddEventSink(HitEventHandler::GetSingleton());
		logger::info("Handler Installed: On Hit");
    }
};

// =============================================================
// Dynamic Tempering and Enchanting
// =============================================================
struct NearbyObjects {
    std::vector<RE::TESObjectREFR*> containers;
    std::vector<RE::Actor*> npcs;
    std::vector<RE::TESObjectREFR*> equipment; // world references that are weapons/armor
};

static bool IsPlayerIndoors(RE::Actor* player)
{
    if (!player) return false;
    auto cell = player->GetParentCell();
    if (!cell) return false;
    return cell->IsInteriorCell();
}

static bool IsPlayerOwned(RE::Actor* player) {
    if (!player) return false;
    auto cell = player->GetParentCell();
    if (!cell) return false;

	// Get owners and verify if the player is the owner
    auto ownerActor = cell->GetActorOwner();
	auto ownerFaction = cell->GetFactionOwner();
	if ((ownerActor && ownerActor == player->GetActorBase()) || (ownerFaction && ownerFaction == Utility::GetSingleton()->playerFaction)) return true;
	
	return false;
}

static void ProcessReference(RE::TESObjectREFR* ref, NearbyObjects& result) {
	if (!ref) return;

    auto* base = ref->GetBaseObject();
    if (!base) return;

    const RE::FormID id = ref->GetFormID();
    if (!id) return;

	// Containers
	if (base->formType == RE::FormType::Container)
		result.containers.push_back(ref);
	
	// Actors
	if (auto actor = ref->As<RE::Actor>())
		result.npcs.push_back(actor);
}

NearbyObjects GetNearbyObjects(RE::Actor* player) {
    NearbyObjects result;

	// Dont process player owned cells
    if (!player || IsPlayerOwned(player)) return result;

	// Set the radius based on if the player is inside or outside
    float radius = IsPlayerIndoors(player) ? 2000.0f : 7000.0f; // 20m indoors, 70m outdoors
    RE::NiPoint3 origin = player->GetPosition();
	
	// Processing function
    auto processRecord = [&](RE::TESObjectREFR* a_ref) {
        if (!a_ref || a_ref->IsDisabled() || a_ref->IsDeleted()) 
            return RE::BSContainer::ForEachResult::kContinue;

        ProcessReference(a_ref, result);
        return RE::BSContainer::ForEachResult::kContinue;
    };

    // Path 1: interior cell fast-path
    if (auto* playerCell = player->GetParentCell(); playerCell && playerCell->IsAttached()) {
        playerCell->ForEachReferenceInRange(origin, radius, processRecord);
        return result;
    }

	// Path 2: exterior grid walk
	auto* tes = RE::TES::GetSingleton();
	if (const auto gridLength = tes->gridCells ? tes->gridCells->length : 0; gridLength > 0) {

		// Build AABB
		const float searchMaxY = origin.y + radius;
		const float searchMinY = origin.y - radius;
		const float searchMaxX = origin.x + radius;
		const float searchMinX = origin.x - radius;

		// Walk through the grid
		for (std::uint32_t x = 0; x < gridLength; ++x) {
			for (std::uint32_t y = 0; y < gridLength; ++y) {
				auto* cell = tes->gridCells->GetCell(x, y);
            	if (!cell || !cell->IsAttached()) continue;

				auto* coords = cell->GetCoordinates();
				if (!coords) continue;

				// Each exterior cell is 4096 units wide
				const float cellX = coords->worldX;
				const float cellY = coords->worldY;
				if (cellX + 4096.0f < searchMinX || cellX > searchMaxX) continue;
				if (cellY + 4096.0f < searchMinY || cellY > searchMaxY) continue;

				// Process the record
				cell->ForEachReferenceInRange(origin, radius, processRecord);
			}
		}
		return result;
	} 

	// Path 3: fallback — neither path above was taken
	tes->ForEachReference([&](RE::TESObjectREFR* a_ref) {
		ProcessReference(a_ref, result);
		return RE::BSContainer::ForEachResult::kContinue;
	});

	return result;
}

static void TemperItem(FoundEquipData* equipData, int actorLevel, bool isVendor = false, bool isBoss = false) {
	auto* setting = Settings::GetSingleton(); 

	// Set the temper chance based on if we are in a boss location or if this is a vendor container
	int chanceTemper = setting->ED_Temper_Chance;
	if (isVendor)
		chanceTemper = setting->ED_Temper_VendorChance;
	else if (isBoss)
		chanceTemper = setting->ED_Temper_BossChance;

	if (Probability::Int(chanceTemper))
		equipData->SetItemHealthPercentCapped(Random::Double(10001.0, 10099.0 + ((actorLevel + 10) * 100)) * 0.0001);
}

static void EnchantItem(FoundEquipData* equipData, RE::TESObjectREFR* ref, int actorLevel, bool isVendor = false, bool isBoss = false) {
	auto* setting = Settings::GetSingleton(); 

	int chanceEnchant = setting->ED_Enchant_Chance;
	if (isVendor)
		chanceEnchant = setting->ED_Enchant_VendorChance;
	else if (isBoss)
		chanceEnchant = setting->ED_Enchant_BossChance;

	if (Probability::Int(chanceEnchant))
		equipData->SetItemEnchantment(actorLevel, ref);
}

static void ProcessInventory(RE::TESObjectREFR* ref) {
	if (!ref || ref->IsPlayerRef() || ref->IsPlayer()) return;

    // Only process attached refs
    auto* cell = ref->GetParentCell();
    if (!cell || !cell->IsAttached()) return;

	// Guard: don't touch if the ref is currently linked to an active container menu (best-effort)
    auto* ui = RE::UI::GetSingleton();
    if (ui && ui->IsMenuOpen(RE::BarterMenu::MENU_NAME)) return;

	// Get Utility Script
	auto* utility = Utility::GetSingleton();
	auto* setting = Settings::GetSingleton(); 

	// Get the level of the Actor or the Player
	int level = utility->GetPlayer()->GetLevel();
	if (RE::Actor* actor = ref->As<RE::Actor>()) {
		if (actor->IsPlayerTeammate()) return;
		else level = actor->GetLevel();
	}

	// Get the inventory changes, return if there are none
	RE::InventoryChanges* invChanges = ref->GetInventoryChanges();
	if (!invChanges || !invChanges->entryList) return;

	// Check for Vendor Chest or Boss Lair
    const bool isVendor = utility->ObjectIsVendor(ref);
    const bool isBoss = utility->LocationIsBoss(ref->extraList);

	// Loop through all items in inventory
	for (const auto& entry : *invChanges->entryList) {
		if (!entry || !entry->object || !entry->extraLists) continue;

		// Process Items with Extra Data
		for (auto& entryData : *entry->extraLists) {
			if (entryData) {
				// Dont process what we've already processed
				FoundEquipData equipData(entry->GetObject(), entryData);
				if (!equipData.CanProcess()) continue;

				// Temper the Item
				if (setting->ED_Temper_Enabled && equipData.CanTemper() && !equipData.IsTempered())
					TemperItem(&equipData, level, isVendor, isBoss);

				// Enchant the Item
				if (setting->ED_Enchant_Enabled && equipData.CanEnchant() && !equipData.IsEnchanted())
					EnchantItem(&equipData, ref, level, isVendor, isBoss);

				// Process the Item so we dont run this again
				equipData.ProcessItem();
			}
		}
	}
}

static void DynamicTemperEnchant() {
	// Do not process player owned locations
	auto* player = Utility::GetSingleton()->GetPlayer();
	if (player && IsPlayerOwned(player)) return;

	// Get nearby NPCs and Containers
	NearbyObjects nearby = GetNearbyObjects(player);

	// Process Containers, Actors and Equipment
	for (RE::TESObjectREFR* container : nearby.containers)
		ProcessInventory(container);
	for (RE::Actor* npc : nearby.npcs)
		ProcessInventory(npc);
}

// =============================================================
// Break System Handler
// =============================================================
static void EquipObject(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, const RE::ObjectEquipParams& a_objectEquipParams) {
	if (a_actor && a_object && !a_objectEquipParams.forceEquip) {

		// If it has the broken keyword, stop it from equipping
		FoundEquipData eqD(a_object, a_objectEquipParams.extraDataList);
		if (eqD.IsBroken() && !Settings::GetSingleton()->ED_BreakDisabled) { 
			if (a_actor == Utility::GetSingleton()->GetPlayer()) {
				auto msg = std::format("{} is broken and cannot be equipped", a_object->GetName());
				Utility::GetSingleton()->ShowNotification(msg, false, "VOCShoutImpactDisarm");
			}
			return;
		 }
	}
	return _EquipObject(a_manager, a_actor, a_object, a_objectEquipParams);
}

// =============================================================
// Update Hook
// =============================================================
static std::int32_t OnUpdate() {
	if (!RE::UI::GetSingleton()->GameIsPaused()) {

		// Clear out the actor list
		ClearActor();

		// Run the dynamic system every second
		if (g_deltaTime > 0) {
			lastTime += g_deltaTime;
			if (lastTime >= 1.0f) {
				if (Settings::GetSingleton()->ED_Temper_Enabled || Settings::GetSingleton()->ED_Enchant_Enabled) {
					DynamicTemperEnchant();
				}
				lastTime -= 1.0f;
			}
		}
	}

	return _OnUpdate();
}

namespace Events {
	inline static REL::Relocation<std::uintptr_t> On_Update_Hook{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xC26) };
	inline static REL::Relocation<std::uintptr_t> EquipObject_Hook{ REL::RelocationID(37938, 38894), REL::Relocate(0xE5, 0x170) };

	void Init(void) {
		HitEventHandler::Register();
		
		// Install hooks
		auto& trampoline = SKSE::GetTrampoline();
		_OnUpdate = trampoline.write_call<5>(On_Update_Hook.address(), OnUpdate);
		logger::info("Hook Installed: On Update");
		_EquipObject = trampoline.write_call<5>(EquipObject_Hook.address(), EquipObject);
		logger::info("Hook Installed: On Equip");

	}
}
