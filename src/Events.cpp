#undef GetObject

#include "Events.h"
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"
#include <unordered_set>
#include <set>
#include <array>
#include <random>

// =============================================================
// Actor Hit Throttle
// =============================================================
std::unordered_set<RE::Actor*> actorList;
std::mutex actorLock;
bool skip = false;

static bool AddActor(RE::Actor* actor) {
	std::lock_guard<std::mutex> guard(actorLock);
	return actorList.insert(actor).second ? (skip = true, true) : false;
}

static void ClearActor() {
	std::lock_guard<std::mutex> guard(actorLock);
	if (skip) skip = false;
	else actorList.clear();
}

// =============================================================
// Random Functions
// =============================================================
static bool Probability(int percentChance) {
    if (percentChance <= 0) return false;
    if (percentChance >= 100) return true;

	thread_local std::mt19937 mt{ std::random_device{}() }; 
	std::uniform_int_distribution<int> dist(0, 99);
    int roll = dist(mt);

    return roll < percentChance;
}

static bool Probability(double percentChance) {
    if (percentChance <= 0.0) return false;
    if (percentChance >= 100.0) return true;

	thread_local std::mt19937 mt{ std::random_device{}() }; 
	std::uniform_real_distribution<double> dist(0.0, 100.0);
    double roll = dist(mt);

    return roll < percentChance;
}

static double GetRandom(double a, double b) {
	thread_local std::mt19937 mt{ std::random_device{}() }; 
	std::uniform_real_distribution<> score(a, b);
	return score(mt) * 0.0001;
}

// =============================================================
// Temper Decay Functions
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

	// Update the name and reset the temper status
	eqD->SetBrokenName();
	eqD->SetItemHealthPercent(utility->MinHealth);
}

static void TemperDecay(FoundEquipData* eqD, RE::Actor* actor, bool powerAttack) {
	auto utility = Utility::GetSingleton();
	auto setting = Settings::GetSingleton();

	if (!eqD->baseForm || !AddActor(actor)) return;
	if (eqD->baseForm == utility->Unarmed) return;

	// Get current health percent
	float itemHealthPercent = eqD->GetItemHealthPercent();

	// --- Breack Chance ---
	if (utility->NormalizedHealth(itemHealthPercent) <= (setting->ED_BreakThreshold / 1000.0f)) {
		float chance = setting->GetBreakChance(eqD->baseForm);

		// Apply modifiers
		if (chance != 0.0 && eqD->CanBreak()) {

			// Increased Durability
			if (setting->ED_IncreasedDurability && itemHealthPercent > utility->MinimumHealth())
				chance *= 1.0 - (utility->NormalizedHealth(itemHealthPercent) / (setting->ED_BreakThreshold / 1000.0f));

			// Power Attack
			if (powerAttack) 
				chance *= setting->ED_Break_PowerAttack;

			// Follower/NPC Multiplier
			if (actor != utility->GetPlayer())
				chance *= actor->IsPlayerTeammate() ? setting->ED_Break_FollowerMulti : setting->ED_Break_NPCMulti;

			// Check to see if we break
			if (Probability(chance)) {
				RemoveEquipment(eqD, actor);
				return;
			}
		}
	}

	// --- Degradation ---
	if (itemHealthPercent <= utility->MinimumHealth()) return;

	double rate = setting->GetDegradationRate(eqD->baseForm);
	if (rate == 0.0) return;

	// Power Attack
	if (powerAttack)
		rate *= setting->ED_Degrade_PowerAttack;

	// Follower/NPC Multiplier
	if (actor != utility->GetPlayer())
		rate *= actor->IsPlayerTeammate() ? setting->ED_Degrade_FollowerMulti : setting->ED_Degrade_NPCMulti;

	// Subtract the health and round to 5 decimal places
	itemHealthPercent -= GetRandom(rate, std::pow(rate + 1.0, 2.0));
	itemHealthPercent = std::round(itemHealthPercent * 100000.0f) / 100000.0f;

	// The default health of an item is always one, so it cant go lower
	if (itemHealthPercent < utility->MinimumHealth()) itemHealthPercent = utility->MinimumHealth();

	// Set the new health of the item
	eqD->SetItemHealthPercent(itemHealthPercent);
}

// =============================================================
// On Hit: Decay Equipment
// =============================================================
class HitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {
public:
	std::mutex container_mutex;

    static HitEventHandler* GetSingleton() {
        static HitEventHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override {
		if (!a_event) return RE::BSEventNotifyControl::kContinue;

		auto* settings = Settings::GetSingleton();
        auto* utility = Utility::GetSingleton();

		// Defender
		if (a_event->target && a_event->target->formType == RE::FormType::ActorCharacter) {
			RE::Actor* actor = a_event->target->As<RE::Actor>();
			if (actor == utility->GetPlayer() || settings->ED_OnlyPlayer) {
				RE::InventoryChanges *exChanges = actor->GetInventoryChanges();
				if (exChanges) {
					RE::TESForm* form = RE::TESForm::LookupByID(a_event->source);

					// If the attack did not come from a staff
					if (form && ((form->formType == RE::FormType::Weapon && !form->As<RE::TESObjectWEAP>()->IsStaff()) || (form->formType == RE::FormType::Armor && form->As<RE::TESObjectARMO>()->IsShield()))) {
						bool powerattack = a_event->flags.any(RE::TESHitEvent::Flag::kPowerAttack);

						// Decay weapon/shield if it blocked the attack
						if (a_event->flags.any(RE::TESHitEvent::Flag::kHitBlocked)) {
							FoundEquipData eqD_armor = FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
							if (eqD_armor.baseForm) {
								TemperDecay(&eqD_armor, actor, powerattack);
							} else {
								if (RE::TESForm* weap = actor->GetEquippedObject(false)) {
									if (weap->IsWeapon() && !weap->As<RE::TESObjectWEAP>()->IsBound()) {
										FoundEquipData eqD_armor = FindEquippedWeapon(exChanges, weap, false);
										TemperDecay(&eqD_armor, actor, powerattack);
									}
								}
							}

						// Decay armor, amrmor slots are shuffled for decay loss
						} else {
							std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4> slots = { RE::BGSBipedObjectForm::BipedObjectSlot::kHead, RE::BGSBipedObjectForm::BipedObjectSlot::kBody, RE::BGSBipedObjectForm::BipedObjectSlot::kHands, RE::BGSBipedObjectForm::BipedObjectSlot::kFeet };
							ShuffleSlots(&slots);
							for (RE::BGSBipedObjectForm::BipedObjectSlot slot : slots) {

								// Find the first piece of armor, break after it gets decayed
								FoundEquipData eqD_armor = FindEquippedArmor(exChanges, slot);
								if (eqD_armor.baseForm) {
									TemperDecay(&eqD_armor, actor, powerattack);
									break;
								} else if (slot == RE::BGSBipedObjectForm::BipedObjectSlot::kHead) {
									eqD_armor = FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHair);
									if (eqD_armor.baseForm) {
										TemperDecay(&eqD_armor, actor, powerattack);
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		// Attacker
		if (a_event->cause && a_event->cause->formType == RE::FormType::ActorCharacter) {
			RE::Actor* actor = a_event->cause->As<RE::Actor>();
			if (actor == utility->GetPlayer() || settings->ED_OnlyPlayer) {
				RE::InventoryChanges *exChanges = actor->GetInventoryChanges();
				if (exChanges) {
					
					// Get the source object of the attack
					if (RE::TESForm* form = RE::TESForm::LookupByID(a_event->source)) {
						bool powerattack = (a_event->flags.any(RE::TESHitEvent::Flag::kPowerAttack));

						// Was the source of the attack a weapon
						if (form->IsWeapon()) {
							RE::TESObjectWEAP* weap = form->As<RE::TESObjectWEAP>();

							// We do not decay staffs or bound weapons
							if (!weap->IsStaff() && !weap->IsBound()) {

								// Weapon in Right Hand
								if (form == actor->GetEquippedObject(false)) {

									// Verify that we are not using a left handed weapon with a left handed attack
									if (form == actor->GetEquippedObject(true)) {
										bool bLeftHandAttack;
										if (actor->GetGraphVariableBool("bLeftHandAttack", bLeftHandAttack) && bLeftHandAttack) {
											FoundEquipData eqD_weapon = FindEquippedWeapon(exChanges, form, true);
											TemperDecay(&eqD_weapon, actor, powerattack);
										} else {
											FoundEquipData eqD_weapon = FindEquippedWeapon(exChanges, form, false);
											TemperDecay(&eqD_weapon, actor, powerattack);
										}
									// Use the right hand
									} else {
										FoundEquipData eqD_weapon = FindEquippedWeapon(exChanges, form, false);
										TemperDecay(&eqD_weapon, actor, powerattack);
									}
								// Weapon in Left Hand (Can't be in the left hand without something in the right) 
								} else if (form == actor->GetEquippedObject(true)) {
									FoundEquipData eqD_weapon = FindEquippedWeapon(exChanges, form, true);
									TemperDecay(&eqD_weapon, actor, powerattack);
								}
									
							}

						// Was the source of the attack a shield
						} else if (form->IsArmor()) {
							if (form->As<RE::TESObjectARMO>()->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield)) {
								FoundEquipData eqD_weapon = FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
								TemperDecay(&eqD_weapon, actor, powerattack);
							}
						}
					}
				}
			}
		}

        return RE::BSEventNotifyControl::kContinue;
    }

    static void Register() {
        RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
        eventHolder->AddEventSink(HitEventHandler::GetSingleton());
		logger::info("Handler Installed: On Hit");
    }

private:
	static void ShuffleSlots(std::array<RE::BGSBipedObjectForm::BipedObjectSlot, 4> *slots) {
		thread_local std::mt19937 mt{ std::random_device{}() }; 
		std::shuffle(slots->begin(), slots->end(), mt);
	}
};

// =============================================================
// Dynamic Tempering and Enchanting
// =============================================================
struct ExtraProcessedFlag : RE::BSExtraData
{
    inline static constexpr auto TYPE = RE::ExtraDataType::kNone; // or any unused type
    ExtraProcessedFlag() = default;
};

struct NearbyObjects {
    std::vector<RE::TESObjectREFR*> containers;
    std::vector<RE::Actor*> npcs;
    std::vector<RE::TESObjectREFR*> equipment; // world references that are weapons/armor
};

inline static bool IsPlayerIndoors(RE::Actor* player)
{
    if (!player) return false;
    auto cell = player->GetParentCell();
    if (!cell) return false;
    return cell->IsInteriorCell();
}

inline static bool IsPlayerOwned(RE::Actor* player) {
    if (!player) return false;
    auto cell = player->GetParentCell();
    if (!cell) return false;

	// Get owners and verify if the player is the owner
    auto ownerActor = cell->GetActorOwner();
	auto ownerFaction = cell->GetFactionOwner();
	if ((ownerActor && ownerActor == player->GetActorBase()) || (ownerFaction && ownerFaction == Utility::GetSingleton()->playerFaction)) return true;
	
	return false;
}

inline static void ProcessReference(RE::TESObjectREFR* ref, NearbyObjects& result) {
	if (!ref) return;

    auto* base = ref->GetBaseObject();
    if (!base) return;

    auto* setting = Settings::GetSingleton();
    const RE::FormID id = ref->GetFormID();
    if (!id) return;

	// Containers
	if (ref->GetBaseObject() && ref->GetBaseObject()->formType == RE::FormType::Container) {
		result.containers.push_back(ref);
	} 
	
	if (auto actor = ref->As<RE::Actor>()) {
		result.npcs.push_back(actor);
	}
}

NearbyObjects GetNearbyObjects(RE::Actor* player) {
    NearbyObjects result;
    if (!player) return result;
	if (IsPlayerOwned(player)) return result;

    float radius = IsPlayerIndoors(player) ? 2000.0f : 7000.0f; // 20m indoors, 70m outdoors
    float radiusSq = radius * radius;
    RE::NiPoint3 origin = player->GetPosition();
	auto* tes = RE::TES::GetSingleton();

    // Prefer the player's actual parent cell (more correct & safer for interiors)
    if (auto* playerCell = player->GetParentCell(); playerCell && playerCell->IsAttached()) {
        playerCell->ForEachReferenceInRange(origin, radius, [&](RE::TESObjectREFR* a_ref) {
            ProcessReference(a_ref, result);
            return RE::BSContainer::ForEachResult::kContinue;
        });
        return result;
    }
	if (const auto gridLength = tes->gridCells ? tes->gridCells->length : 0; gridLength > 0) {
		const float searchMaxY = origin.y + radiusSq;
		const float searchMinY = origin.y - radiusSq;
		const float searchMaxX = origin.x + radiusSq;
		const float searchMinX = origin.x - radiusSq;

		for (std::uint32_t x = 0; x < gridLength; ++x) {
			for (std::uint32_t y = 0; y < gridLength; ++y) {
				if (const auto cell = tes->gridCells->GetCell(x, y); cell && cell->IsAttached()) {
					if (const auto cellCoords = cell->GetCoordinates(); cellCoords) {
						const RE::NiPoint2 worldPos{ cellCoords->worldX, cellCoords->worldY };
						if (worldPos.x < searchMaxX && (worldPos.x + 4096.0f) > searchMinX &&
							worldPos.y < searchMaxY && (worldPos.y + 4096.0f) > searchMinY) {
							cell->ForEachReferenceInRange(origin, radiusSq, [&](RE::TESObjectREFR* a_ref) {
								ProcessReference(a_ref, result);
								return RE::BSContainer::ForEachResult::kContinue;
							});
						}
					}
				}
			}
		}
		return result;
	} 

	tes->ForEachReference([&](RE::TESObjectREFR* a_ref) {
		ProcessReference(a_ref, result);
		return RE::BSContainer::ForEachResult::kContinue;
	});

	return result;
}

static void ProcessItem(FoundEquipData* equipData, int actorLevel, bool isVendor = false, bool isBoss = false) {
	auto* setting = Settings::GetSingleton(); 

	// Temper Process
	if (actorLevel != 0) {
		if (setting->ED_Temper_Enabled && !equipData->IsTempered()) {
			// Set the temper chance based on if we are in a boss location or if this is a vendor container
			int chanceTemper = setting->ED_Temper_Chance;
			if (isVendor)
				chanceTemper = setting->ED_Temper_VendorChance;
			else if (isBoss)
				chanceTemper = setting->ED_Temper_BossChance;

			if (Probability(chanceTemper))
				equipData->SetItemHealthPercent(GetRandom(10001.0, 10001.0 + (actorLevel + 10) * 100));
		}

		// Enchant Process
		if (setting->ED_Enchant_Enabled && !equipData->IsEnchanted()) {
			int chanceEnchant = setting->ED_Enchant_Chance;
			if (isVendor)
				chanceEnchant = setting->ED_Enchant_VendorChance;
			else if (isBoss)
				chanceEnchant = setting->ED_Enchant_BossChance;

			if (Probability(chanceEnchant))
				equipData->SetItemEnchantment(actorLevel);
	}
	} 

	if (!equipData->HasBeenProcessed())
		equipData->ProcessItem();
}

static void ProcessInventory(RE::TESObjectREFR* ref) {
	if (!ref || ref->IsPlayerRef() || ref->IsPlayer()) return;

    // Only process attached refs
    auto* cell = ref->GetParentCell();
    if (!cell || !cell->IsAttached()) return;

	// Guard: don't touch if the ref is currently linked to an active container menu (best-effort)
    auto* ui = RE::UI::GetSingleton();
    if (ui && ui->IsMenuOpen(RE::BarterMenu::MENU_NAME)) return;

	// Get the inventory changes, return if there are none
	RE::InventoryChanges* invChanges = ref->GetInventoryChanges();
	if (!invChanges || !invChanges->entryList) return;

	// Vendor/boss check
	RE::ExtraLocationRefType* xRefType = nullptr;
    if (ref->extraList.HasType(RE::ExtraDataType::kLocationRefType))
        xRefType = static_cast<RE::ExtraLocationRefType*>(ref->extraList.GetByType(RE::ExtraDataType::kLocationRefType));

    const bool isVendor = (ref->GetBaseObject() && ref->GetBaseObject()->formType == RE::FormType::Container && Settings::GetSingleton()->IsVendorContainer(ref));
    const bool isBoss = (xRefType && (xRefType->locRefType == Utility::GetSingleton()->locationBoss || xRefType->locRefType == Utility::GetSingleton()->locationBossContainer));

	// Get the level of the Actor or the Player
	int level = 0;
	if (RE::Actor* actor = ref->As<RE::Actor>()) {
		if (!actor->IsPlayer() && !actor->IsPlayerTeammate())
			level = actor->GetLevel();
	} else {
		level = Utility::GetSingleton()->GetPlayer()->GetLevel();
	}

	// Loop through all items in list
	for (const auto& entry : *invChanges->entryList) {
		if (!entry || !entry->object) continue;

		// We need to check the initial object
		FoundEquipData equipData(entry->GetObject());
		if (!equipData.CanTemper()) continue;

		// Process Items with Extra Data
		if (entry->extraLists) {
			for (auto& entryData : *entry->extraLists) {
				if (entryData) {
					// Get the entry, process it and return the new extra data
					equipData.objectData = entryData;
					if (!equipData.HasBeenProcessed()) {
						ProcessItem(&equipData, level, isVendor, isBoss);
						entryData = equipData.objectData;
					}
				} else {
					// Create a new list if there is not one already
					ProcessItem(&equipData, level, isVendor, isBoss);
					entry->AddExtraList(equipData.objectData);
				}
			}
		}
	}
}

static void DynamicTemperEnchant() {
	// Prune Cache
	auto* setting = Settings::GetSingleton();

	// Do not process player owned locations
	auto* player = Utility::GetSingleton()->GetPlayer();
	if (player && IsPlayerOwned(player)) return;

	// Get nearby NPCs and Containers
	NearbyObjects nearby = GetNearbyObjects(player);

	// Process Containers, Actors and Equipment
	for (RE::TESObjectREFR* container : nearby.containers) ProcessInventory(container);
	for (RE::Actor* npc : nearby.npcs) ProcessInventory(npc);
}

// =============================================================
// Break System Handler
// =============================================================
class ItemCraftedHandler : public RE::BSTEventSink<RE::ItemCrafted::Event> {
public:
	static ItemCraftedHandler* GetSingleton() {
		static ItemCraftedHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::ItemCrafted::Event* a_event, RE::BSTEventSource<RE::ItemCrafted::Event>* a_eventSource) override {
		if (!a_event)
			return RE::BSEventNotifyControl::kContinue;

		if (RE::TESForm* item = a_event->item; item) {
			RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
			RE::TESObjectREFR::InventoryItemMap playerInventory = player->GetInventory();

			RE::InventoryChanges* invChanges = player->GetInventoryChanges(item);
			for (RE::InventoryEntryData* entry : *invChanges->entryList) {
				if (!entry || !entry->extraLists) continue;

				for (auto* dataList : *entry->extraLists) {
					FoundEquipData eqD(entry->GetObject(), dataList);
					eqD.refForm = entry->GetObject();
					auto utility = Utility::GetSingleton();
					
					// Set health if tempered
					if (float health = eqD.GetItemHealthRounded()) {
						int tenths = static_cast<int>(std::round(health * 10.0f));

						if (tenths >= 11 && tenths <= 30 && std::fabs(health * 10.0f - tenths) < 1e-6f)
							eqD.SetItemHealthPercent(health + 0.099f - utility->StepToMin);

						if (eqD.IsBroken())
							eqD.SetFixedName();
					}		
				}
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register() {
		using GetEventSource_t = RE::BSTEventSource<RE::ItemCrafted::Event>* (*)();
		auto* eventHolder = REL::Relocation<GetEventSource_t>(RE::Offset::ItemCrafted::GetEventSource)();
		eventHolder->AddEventSink(ItemCraftedHandler::GetSingleton());
		logger::info("Handler Installed: Crafted Item");
	}
};

static void EquipObject(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, const RE::ObjectEquipParams& a_objectEquipParams) {
	if (a_actor && a_object && !a_objectEquipParams.forceEquip) {

		// If it has the broken keyword, stop it from equipping
		FoundEquipData eqD(a_object, a_objectEquipParams.extraDataList);
		if (eqD.IsBroken()) { 
			auto msg = std::format("{} is broken and cannot be equipped", a_object->GetName());
			Utility::GetSingleton()->ShowNotification(msg, false, "VOCShoutImpactDisarm");
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
		ItemCraftedHandler::Register();

		// Install hooks
		auto& trampoline = SKSE::GetTrampoline();
		_OnUpdate = trampoline.write_call<5>(On_Update_Hook.address(), OnUpdate);
		logger::info("Hook Installed: On Update");
		_EquipObject = trampoline.write_call<5>(EquipObject_Hook.address(), EquipObject);
		logger::info("Hook Installed: On Equip");

	}
}