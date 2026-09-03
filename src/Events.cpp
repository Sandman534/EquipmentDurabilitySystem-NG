#undef GetObject
#include "Events.h"
#include "FoundEquipData.h"
#include "DurabilityMenu.h"
#include "Settings.h"
#include "Utility.h"
#include "Translation.h"
#include <algorithm>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <set>
#include <array>

// =============================================================
// Processed Inventory Cache
// =============================================================
namespace {
	constexpr auto kSerializationID = 'EDSN';
	constexpr auto kProcessedReferencesRecord = 'PREF';
	constexpr std::uint32_t kProcessedReferencesVersion = 1;
	constexpr std::uint32_t kMaximumSerializedReferences = 50'000'000;

	class ProcessedReferenceCache {
	public:
		static ProcessedReferenceCache& GetSingleton() {
			static ProcessedReferenceCache singleton;
			return singleton;
		}

		bool Contains(RE::FormID formID) const {
			std::lock_guard guard(lock);
			return persistentProcessed.contains(formID);
		}

		void Insert(RE::FormID formID) {
			std::lock_guard guard(lock);
			persistentProcessed.insert(formID);
		}

		bool CanProcess(RE::FormID formID) {
			std::lock_guard guard(lock);
			if (persistentProcessed.contains(formID))
				return false;
			
			persistentProcessed.insert(formID);
			return true;
		}

		void Invalidate(const RE::TESObjectREFR* ref) {
			if (!ref) return;

			std::lock_guard guard(lock);
			persistentProcessed.erase(ref->GetFormID());
		}

		void Save(SKSE::SerializationInterface* serialization) {
			std::lock_guard guard(lock);

			std::vector<RE::FormID> serialized;
			serialized.reserve(persistentProcessed.size());
			for (const auto formID : persistentProcessed) {
				if (formID < 0xFF000000) serialized.push_back(formID);
			}
			std::ranges::sort(serialized);

			if (!serialization->OpenRecord(kProcessedReferencesRecord, kProcessedReferencesVersion)) {
				logger::error("Failed to open processed-reference serialization record");
				return;
			}

			const auto count = static_cast<std::uint32_t>(serialized.size());
			if (!serialization->WriteRecordData(count)) {
				logger::error("Failed to write processed-reference count");
				return;
			}

			if (count > 0 && !serialization->WriteRecordData(
					serialized.data(), count * static_cast<std::uint32_t>(sizeof(RE::FormID)))) {
				logger::error("Failed to write processed-reference data");
				return;
			}

			logger::info("Saved {} processed references ({} dynamic references omitted)", count, persistentProcessed.size() - serialized.size());
		}

		void Load(SKSE::SerializationInterface* serialization) {
			Revert();

			std::uint32_t type = 0;
			std::uint32_t version = 0;
			std::uint32_t length = 0;
			while (serialization->GetNextRecordInfo(type, version, length)) {
				if (type != kProcessedReferencesRecord) continue;
				if (version != kProcessedReferencesVersion) {
					logger::warn("Ignoring unsupported processed-reference record version {}", version);
					continue;
				}

				std::uint32_t count = 0;
				if (serialization->ReadRecordData(count) != sizeof(count) ||
					count > kMaximumSerializedReferences ||
					length != sizeof(count) + count * sizeof(RE::FormID)) {
					logger::error("Ignoring invalid processed-reference record");
					continue;
				}

				std::vector<RE::FormID> saved(count);
				const auto byteCount = count * static_cast<std::uint32_t>(sizeof(RE::FormID));
				if (byteCount > 0 && serialization->ReadRecordData(saved.data(), byteCount) != byteCount) {
					logger::error("Failed to read processed-reference data");
					continue;
				}

				std::unordered_set<RE::FormID> resolved;
				resolved.reserve(saved.size());
				for (const auto savedID : saved) {
					RE::FormID resolvedID = 0;
					if (serialization->ResolveFormID(savedID, resolvedID) && resolvedID < 0xFF000000) {
						resolved.insert(resolvedID);
					}
				}

				{
					std::lock_guard guard(lock);
					persistentProcessed = std::move(resolved);
				}
				logger::info("Loaded {} of {} processed references", persistentProcessed.size(), count);
			}
		}

		void Revert() {
			std::lock_guard guard(lock);
			persistentProcessed.clear();
		}

	private:
		std::unordered_set<RE::FormID> persistentProcessed;
		mutable std::mutex lock;
	};

	void SaveProcessedReferences(SKSE::SerializationInterface* serialization) {
		ProcessedReferenceCache::GetSingleton().Save(serialization);
	}

	void LoadProcessedReferences(SKSE::SerializationInterface* serialization) {
		ProcessedReferenceCache::GetSingleton().Load(serialization);
	}

	void RevertProcessedReferences(SKSE::SerializationInterface*) {
		ProcessedReferenceCache::GetSingleton().Revert();
	}
}

// =============================================================
// Player Animations
// =============================================================
struct PlayerAttackState {
	std::mutex mutex;
	std::deque<bool> pendingHands;
	std::uint32_t hitFrameCount{ 0 };
	bool parryAttackPending{ false };
	std::chrono::steady_clock::time_point lastBlockStop;
	std::chrono::steady_clock::time_point lastEventTime;
};

struct PlayerGraphEventHook {
	static inline PlayerAttackState playerAttackState;

	static RE::BSEventNotifyControl ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_sink, RE::BSAnimationGraphEvent *a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource) {
		if (!a_event)
			return _ProcessEvent(a_sink, a_event, a_eventSource);

		// On weapon draw, display menu
		if (a_event->tag == "weaponDraw" || a_event->tag == "weaponSheathe") {
			if (auto durability = DurabilityMenu::GetSingleton()) {
				if (a_event->tag == "weaponDraw") durability->sheathActivated = true;
				else if (a_event->tag == "weaponSheathe") durability->sheathActivated = false;
				durability->MenuState();
			}
		}

		// Left Swing
		if (a_event->tag == "weaponLeftSwing") {
			std::scoped_lock lock{ playerAttackState.mutex };
			playerAttackState.pendingHands.clear();
			playerAttackState.pendingHands.push_back(true);
			playerAttackState.hitFrameCount = 0;
			playerAttackState.lastEventTime = std::chrono::steady_clock::now();
		}

		// Right Swing
		else if (a_event->tag == "weaponSwing") {
			std::scoped_lock lock{ playerAttackState.mutex };
			const auto now = std::chrono::steady_clock::now();
			playerAttackState.pendingHands.clear();
			playerAttackState.pendingHands.push_back(false);
			playerAttackState.hitFrameCount = 0;
			playerAttackState.lastEventTime = now;
			playerAttackState.parryAttackPending = now - playerAttackState.lastBlockStop < std::chrono::milliseconds(100);
		}

		// Two hit events in a row
		else if (a_event->tag == "HitFrame") {
			std::scoped_lock lock{ playerAttackState.mutex };
			++playerAttackState.hitFrameCount;

			// A normal dual-wield attack emits one left swing followed by two
			// hit frames. Queue the right hand for its second hit.
			if (playerAttackState.hitFrameCount == 2 &&
				playerAttackState.pendingHands.size() == 1 &&
				playerAttackState.pendingHands.front()) {
				playerAttackState.pendingHands.push_back(false);
			}
		}

		// Attempt to detect a Dual Wield Left Hand Bash
		else if (a_event->tag == "tailCombatIdle") {
			std::scoped_lock lock{ playerAttackState.mutex };
			playerAttackState.lastBlockStop = std::chrono::steady_clock::now();
			playerAttackState.parryAttackPending = false;
		}

		else if (a_event->tag == "bashExit" || a_event->tag == "bashStop" || a_event->tag == "attackStop") {
			std::scoped_lock lock{ playerAttackState.mutex };
			playerAttackState.parryAttackPending = false;
		}

		return _ProcessEvent(a_sink, a_event, a_eventSource);
	}

	static void Install() {
        REL::Relocation<uintptr_t> AnimEventVtbl_PC{RE::VTABLE_PlayerCharacter[2]};
        _ProcessEvent = AnimEventVtbl_PC.write_vfunc(0x1, ProcessEvent);
		SKSE::log::info("Hook Installed: Player Graph Event");
	}

	static bool ConsumeAttackHand() {
		std::scoped_lock lock{ playerAttackState.mutex };

		if (std::chrono::steady_clock::now() - playerAttackState.lastEventTime > std::chrono::seconds(2)) {
			playerAttackState.pendingHands.clear();
			return false;
		}

		if (playerAttackState.pendingHands.empty())
			return false;

		const bool leftHand = playerAttackState.pendingHands.front();
		playerAttackState.pendingHands.pop_front();
		return leftHand;
	}

	static bool ConsumeParryAttack() {
		std::scoped_lock lock{ playerAttackState.mutex };

		if (!playerAttackState.parryAttackPending ||
			std::chrono::steady_clock::now() - playerAttackState.lastBlockStop > std::chrono::milliseconds(500)) {
			playerAttackState.parryAttackPending = false;
			return false;
		}

		playerAttackState.parryAttackPending = false;
		return true;
	}

	inline static thread_local bool processing = false;
	static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
};

// =============================================================
// Temper / Decay Functions
// =============================================================
static void BreakEquipment(FoundEquipData* eqD, RE::Actor* actor) {
	if (!actor || !eqD->baseForm) return;
	auto utility = Utility::GetSingleton();
	auto setting = Settings::GetSingleton();

    // Player notification
    if (actor == utility->GetPlayer()) {
        eqD->CreateName();
        auto msg = std::vformat(EDTranslation::Translate("Notification.Break"), std::make_format_args(eqD->objectName));
        utility->ShowNotification(msg, false, "VOCShoutImpactDisarm");
    }

	// Unequip or Remove the broken weapon
	if (setting->ED_RemoveEquipment) {
		actor->RemoveItem(eqD->refForm, 1, RE::ITEM_REMOVE_REASON::kRemove, eqD->objectData, nullptr, 0, 0);
	} else {
		if (auto equipMgr = RE::ActorEquipManager::GetSingleton())
			equipMgr->UnequipObject(actor, eqD->refForm, eqD->objectData, 1, nullptr, true, false, true, false, nullptr);

		// Update the name and set the health value to something lower than minimum
		eqD->SetItemHealthPercent(Degredation::kBrokenHealth);
	}

	
}

static void TemperDecay(FoundEquipData* eqD, RE::Actor* actor, bool powerAttack) {
	auto utility = Utility::GetSingleton();
	auto setting = Settings::GetSingleton();

	// Check for system enabled; The item is not unarmed; If the actor is not throttled
	// if (setting->ED_DegradationDisabled || !eqD->CanTemper() || !AddActor(actor)) return;
	if (setting->ED_DegradationDisabled || !eqD->CanTemper()) return;

	// Get current health percent
	float CurrentHealth = eqD->GetItemHealthPercent();
	float BreakThreshold = (setting->ED_BreakThreshold / 1000.0f);
	
	// --- Break Chance ---
	if ((CurrentHealth - Degredation::kMinHealth) <= BreakThreshold && eqD->CanBreak()) {
		auto chance = setting->GetBreakChance(eqD->baseForm, actor);

		// Apply modifiers
		if (chance != 0.0) {

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
				BreakEquipment(eqD, actor);
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
	CurrentHealth -= static_cast<float>(loss);
	CurrentHealth = static_cast<float>(std::round(CurrentHealth * Degredation::kPrecision) / Degredation::kPrecision);

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

	// Left Hand Parry
	RE::TESForm* leftHand = actor->GetEquippedObject(true);
	if (leftHand) {
		if (auto* leftWeapon = leftHand->As<RE::TESObjectWEAP>()) {
			if (!leftWeapon || leftWeapon->IsBound()) return;

			FoundEquipData weaponData = FindEquippedWeapon(changes, actor, leftHand, true);
			TemperDecay(&weaponData, actor, powerAttack);
			return;
		}
	}

	// Right Hand Parry
	RE::TESForm* rightHand = actor->GetEquippedObject(false);
	if (rightHand) {
		if (auto* rightWeapon = rightHand->As<RE::TESObjectWEAP>()) {
			if (!rightWeapon || rightWeapon->IsBound()) return;

			FoundEquipData weaponData = FindEquippedWeapon(changes, actor, rightHand, false);
			TemperDecay(&weaponData, actor, powerAttack);
		}
	}
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

	// Get the form of the weapon
	RE::TESForm* form = RE::TESForm::LookupByID(event->source);

	//====================================
	// Shield Bash
	//====================================
	if (event->flags.any(RE::TESHitEvent::Flag::kBashAttack)) {
		RE::InventoryChanges* changes = actor->GetInventoryChanges();
		if (!changes) return;

		// TESHitEvent::source can be Unarmed for a shield bash, so resolve
		// the equipped shield directly rather than relying on the event form.
		FoundEquipData shield = FindEquippedArmor(changes, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
		if (shield.baseForm) {
			TemperDecay(&shield, actor, powerAttack);
			return;
		}
	}

	//====================================
	// Handle Weapon Attack
	//====================================
	// Get left hand based on graph variable
	bool attackHand = false;
	actor->GetGraphVariableBool("bLeftHandAttack", attackHand);

	// Use the more sure option for players
	if (actor->IsPlayerRef()) {
		attackHand = PlayerGraphEventHook::ConsumeAttackHand();
		if (!attackHand)
			attackHand = PlayerGraphEventHook::ConsumeParryAttack();
	}

	// Find the item and process the hit
	FoundEquipData weaponData = FindEquippedWeapon(actor->GetInventoryChanges(), actor, form, attackHand);
	TemperDecay(&weaponData, actor, powerAttack);
}

class HitEventHandler : public RE::BSTEventSink<RE::TESHitEvent> {
public:
    static HitEventHandler* GetSingleton() {
        static HitEventHandler singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event, [[maybe_unused]] RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override {
		if (!a_event) return RE::BSEventNotifyControl::kContinue;
		if (Settings::GetSingleton()->ED_DegradationDisabled) return RE::BSEventNotifyControl::kContinue;

		// Determine power attack
		bool isPowerAttacking = a_event->cause && a_event->cause->formType == RE::FormType::ActorCharacter && a_event->cause->As<RE::Actor>()->IsPowerAttacking();
		bool powerAttack = a_event->flags.any(RE::TESHitEvent::Flag::kPowerAttack) || isPowerAttacking;

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
// Dynamic Processing
// =============================================================
static bool PlayerOwnedLocation() {
	auto* player = RE::PlayerCharacter::GetSingleton();
	auto* utility = Utility::GetSingleton();

	// No Player
    if (!player)
		return false;

	// No Cell
    auto cell = player->GetParentCell();
    if (!cell)
		return false;

	// Cell Owner and Owner Faction
    auto ownerActor = cell->GetActorOwner();
	auto ownerFaction = cell->GetFactionOwner();
	if ((ownerActor && ownerActor == player->GetActorBase()) || (ownerFaction && ownerFaction == utility->playerFaction))
		return true;

	// Not Palyer Owned
	return false;
}

static bool RollTemper(bool isVendor, bool isBoss) {
	auto* setting = Settings::GetSingleton(); 
	int chanceTemper = setting->ED_Temper_Chance;
	if (isVendor)
		chanceTemper = setting->ED_Temper_VendorChance;
	else if (isBoss)
		chanceTemper = setting->ED_Temper_BossChance;
	return Probability::Int(chanceTemper);
}

static void TemperItem(FoundEquipData* equipData, int actorLevel) {
	equipData->SetItemHealthPercentCapped(static_cast<float>(Random::Double(10001.0, 10001.0 + ((actorLevel + 10) * 100)) * 0.0001));
}

static bool RollEnchant(bool isVendor, bool isBoss) {
	auto* setting = Settings::GetSingleton(); 
	int chanceEnchant = setting->ED_Enchant_Chance;
	if (isVendor)
		chanceEnchant = setting->ED_Enchant_VendorChance;
	else if (isBoss)
		chanceEnchant = setting->ED_Enchant_BossChance;
	return Probability::Int(chanceEnchant);
}

static void EnchantItem(FoundEquipData* equipData, RE::TESObjectREFR* ref, int actorLevel) {
	equipData->SetItemEnchantment(actorLevel, ref);
}

static RE::ExtraDataList* ConstructExtraDataList(void* a_this) {
	using func_t = decltype(&ConstructExtraDataList);
	REL::Relocation<func_t> func{RELOCATION_ID(11437, 11583)};
	return func(a_this);
}

RE::ExtraDataList* CreateNewList() {
    const auto memoryManager = RE::MemoryManager::GetSingleton();
    const auto alloc = memoryManager->Allocate(0x20, 0, false);
    return ConstructExtraDataList(alloc);
}

static RE::ExtraDataList* CloneExtraDataList(RE::TESBoundObject* object, RE::ExtraDataList* sourceList) {
	if (!object || !sourceList) return nullptr;

	RE::InventoryEntryData source(object, sourceList->GetCount());
	source.AddExtraList(sourceList);

	RE::InventoryEntryData copy;
	copy.DeepCopy(source);
	if (!copy.extraLists || copy.extraLists->begin() == copy.extraLists->end()) return nullptr;

	return *copy.extraLists->begin();
}

static void ProcessInventoryChanges(RE::InventoryChanges* inventoryChanges, RE::TESObjectREFR* owner, int level, bool isVendor, bool isBoss) {
	if (!inventoryChanges || !inventoryChanges->entryList) return;

	// Process the inventory
	auto* setting = Settings::GetSingleton();
	bool inventoryModified = false;

	for (const auto& entry : *inventoryChanges->entryList) {
		// Make sure we have an object
		if (!entry || !entry->object) continue;

		// Set the equipment data and determine if the object can be processed
		FoundEquipData equipData(entry->GetObject());
		if (!equipData.CanProcessObject()) continue;

		// Return the total amount of this item in inventory. ExtraDataLists represent subsets of this total.
		const auto totalCount = inventoryChanges->GetCount(entry->GetObject(), [](const RE::InventoryEntryData*) { return true; });
		if (totalCount <= 0) continue;
		std::int32_t explicitCount = 0;

		// Process all items with lists. Use a snapshot to prevent wonkiness
		if (entry->extraLists) {
			// Snapshot the Lists
			std::vector<RE::ExtraDataList*> extraLists;
			for (auto* entryData : *entry->extraLists)
				if (entryData) extraLists.push_back(entryData);

			// Process the lists
			for (auto* entryData : extraLists) {
				const auto entryDataCount = (std::max)(entryData->GetCount(), 0);
				explicitCount += entryDataCount;

				FoundEquipData stack(entry->GetObject(), entryData);
				if (!stack.CanProcessData()) continue;

				// If we have more than one item
				if (entryDataCount > 1) {
					struct ItemRoll { bool temper; bool enchant; };
					std::vector<ItemRoll> successfulRolls;
					successfulRolls.reserve(static_cast<std::size_t>(entryDataCount));

					// determine rolls for the entire stack
					for (std::int32_t i = 0; i < entryDataCount; ++i) {
						const bool temper = setting->ED_Temper_Enabled && stack.CanTemper() &&
							!stack.IsTempered() && RollTemper(isVendor, isBoss);
						const bool enchant = setting->ED_Enchant_Enabled && stack.CanEnchant() &&
							!stack.IsEnchanted() && RollEnchant(isVendor, isBoss);
						if (temper || enchant) successfulRolls.push_back({ temper, enchant });
					}

					// If no rolls were successful
					if (successfulRolls.empty()) continue;

					// Apply roll value function
					auto applyRoll = [&](RE::ExtraDataList* itemList, const ItemRoll& roll) {
						FoundEquipData item(entry->GetObject(), itemList);
						if (roll.temper) TemperItem(&item, level);
						if (roll.enchant) EnchantItem(&item, owner, level);
					};

					// Prepare for clonming
					std::vector<RE::ExtraDataList*> modifiedItems;
					const bool modifyOriginal = successfulRolls.size() == static_cast<std::size_t>(entryDataCount);
					const auto cloneCount = successfulRolls.size() - (modifyOriginal ? 1 : 0);
					modifiedItems.reserve(cloneCount);

					// Clone the ExtraData from the stack before we apply modifications
					for (std::size_t i = 0; i < cloneCount; ++i) {
						auto* clone = CloneExtraDataList(entry->GetObject(), entryData);
						if (!clone) continue;
						clone->SetCount(1);
						applyRoll(clone, successfulRolls[i]);
						modifiedItems.push_back(clone);
					}

					// Take the clones and apply the roll modifiers
					const bool allClonesCreated = modifiedItems.size() == cloneCount;
					if (modifyOriginal && allClonesCreated) {
						entryData->SetCount(1);
						applyRoll(entryData, successfulRolls.back());
					} else
						entryData->SetCount(static_cast<std::uint16_t>(entryDataCount - modifiedItems.size()));

					// Add the new extralist
					for (auto* itemList : modifiedItems) entry->AddExtraList(itemList);
					inventoryModified = inventoryModified || (modifyOriginal && allClonesCreated) || !modifiedItems.empty();
					continue;
				}

				// Temper or Enchant singular items
				if (setting->ED_Temper_Enabled && stack.CanTemper() && !stack.IsTempered() && RollTemper(isVendor, isBoss))
					TemperItem(&stack, level);

				if (setting->ED_Enchant_Enabled && stack.CanEnchant() && !stack.IsEnchanted() && RollEnchant(isVendor, isBoss))
					EnchantItem(&stack, owner, level);

				// We need to track that the inventory was modified
				inventoryModified = true;
			}
		}

		// Anything not accounted for by an ExtraDataList is an implicit/plain
		// item. Give each remaining item its own list and process it separately.
		const auto implicitCount = (std::max)(totalCount - explicitCount, 0);
		for (std::int32_t i = 0; i < implicitCount; ++i) {
			auto* extraList = CreateNewList();
			if (!extraList) continue;

			// Without ExtraCount, this list represents exactly one item.
			entry->AddExtraList(extraList);
			FoundEquipData item(entry->GetObject(), extraList);

			if (setting->ED_Temper_Enabled && item.CanTemper() && RollTemper(isVendor, isBoss))
				TemperItem(&item, level);

			if (setting->ED_Enchant_Enabled && item.CanEnchant() && RollEnchant(isVendor, isBoss))
				EnchantItem(&item, owner, level);

			item.ProcessItem();
			inventoryModified = true;
		}
	}

	// Send the inventory update to the reference
	if (inventoryModified) {
		inventoryChanges->changed = true;
		RE::SendUIMessage::SendInventoryUpdateMessage(owner, nullptr);
	}
}

static void ProcessInventory(RE::TESObjectREFR* ref, bool forceVendor = false, bool bypassCache = false) {
	if (!ref || ref->IsPlayerRef() || ref->IsPlayer()) return;

	// Get Utility and Player
	auto* utility = Utility::GetSingleton();
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) return;

	// Processing level
	int level = player->GetLevel();
	if (RE::Actor* actor = ref->As<RE::Actor>()) {
		if (actor->IsPlayerTeammate()) return;
		else level = actor->GetLevel();
	}

	// Check the process cache to see if we've already processed this NPC
	auto& cache = ProcessedReferenceCache::GetSingleton();
	const auto formID = ref->GetFormID();
	if (!cache.CanProcess(formID) && !bypassCache) return;

	// Check for inventory
	RE::InventoryChanges* invChanges = ref->GetInventoryChanges();
	if (!invChanges || !invChanges->entryList) return;

	// Check for Vendor Chest or Boss Lair
    const bool isVendor = forceVendor || utility->ObjectIsVendor(ref);
    const bool isBoss = utility->ObjectIsBoss(ref);

	ProcessInventoryChanges(invChanges, ref, level, isVendor, isBoss);
}

// =============================================================
// Dynamic Special Processing
// =============================================================
// Process vendor inventories immediately after Skyrim regenerates them
struct InventoryResetHook {
	static void ResetReference(RE::TESObjectREFR* ref, bool leveledOnly) {
		_ResetReference(ref, leveledOnly);

		if (!processing && ref) {
			auto* base = ref->GetBaseObject();
			if (!base || !base->Is(RE::FormType::Container)) return;

			processing = true;
			if (Settings::GetSingleton()->isDynamicEnabled())
				ProcessInventory(ref, false, true);
			processing = false;
		}
	}

	static void ResetActor(RE::Actor* actor, bool leveledOnly) {
		_ResetActor(actor, leveledOnly);

		if (!processing && actor && !actor->IsPlayerRef()) {
			processing = true;
			if (Settings::GetSingleton()->isDynamicEnabled())
				ProcessInventory(actor, false, true);
			processing = false;
		}
	}

	static void Install() {
		constexpr std::size_t resetInventoryIndex = 0x8A;

		REL::Relocation<std::uintptr_t> referenceVtable{ RE::VTABLE_TESObjectREFR[0] };
		_ResetReference = referenceVtable.write_vfunc(resetInventoryIndex, ResetReference);

		REL::Relocation<std::uintptr_t> actorVtable{ RE::VTABLE_Actor[0] };
		_ResetActor = actorVtable.write_vfunc(resetInventoryIndex, ResetActor);

		logger::info("Hook Installed: Inventory Reset");
	}

	inline static thread_local bool processing = false;
	inline static REL::Relocation<decltype(ResetReference)> _ResetReference;
	inline static REL::Relocation<decltype(ResetActor)> _ResetActor;
};

struct BarterMenuHook {
	static RE::UI_MESSAGE_RESULTS ProcessMessage(RE::BarterMenu* menu, RE::UIMessage& message) {

		if (message.type == RE::UI_MESSAGE_TYPE::kShow && Settings::GetSingleton()->isDynamicEnabled()) {
			// Prefer BarterMenu's target
			auto targetHandle = menu->GetTargetRefHandle();
			auto targetPtr = RE::Actor::LookupByHandle(targetHandle);
			auto* vendor = targetPtr.get();
			if (!vendor) {

				// Get the actor to determine its vendor faction
				auto* actorBase = vendor->GetActorBase();
				if (!actorBase) {

					// Process the associated merchant container wherever it may be
					for (const auto& factionRank : actorBase->factions) {
						auto* faction = factionRank.faction;
						if (!faction || !faction->IsVendor()) continue;

						auto* merchantContainer = faction->vendorData.merchantContainer;
						if (merchantContainer)
							ProcessInventory(merchantContainer, true, true);
					}
				}
			}
		}

		return _ProcessMessage(menu, message);
	}

	static void Install() {
		REL::Relocation<std::uintptr_t> vtable{ RE::VTABLE_BarterMenu[0] };
		_ProcessMessage = vtable.write_vfunc(0x4, ProcessMessage);
		logger::info("Hook Installed: Barter Menu");
	}

	inline static REL::Relocation<decltype(ProcessMessage)> _ProcessMessage;
};

// =============================================================
// Dynamic Container/NPC Attach
// =============================================================
// Process on record attach
class ReferenceAttachEventHandler : public RE::BSTEventSink<RE::TESCellAttachDetachEvent> {
public:
	static ReferenceAttachEventHandler* GetSingleton() {
		static ReferenceAttachEventHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::TESCellAttachDetachEvent* event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*) override {		
		// Check for event or player owned location
		if (!event || !event->attached || !event->reference || PlayerOwnedLocation() || !Settings::GetSingleton()->isDynamicEnabled())
			return RE::BSEventNotifyControl::kContinue;


		// Get the FormID of the object, and queue it for processing
		const auto formID = event->reference->GetFormID();
		SKSE::GetTaskInterface()->AddTask([formID]() {
			// Resolve the reference
			auto* ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(formID);
			if (!ref || ref->IsPlayerRef()) return;

			auto* base = ref->GetBaseObject();
			if (!base) return;

			if (ref->As<RE::Actor>() || base->Is(RE::FormType::Container))
				ProcessInventory(ref);
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register() {
		auto* source = RE::ScriptEventSourceHolder::GetSingleton();

		if (source) {
			source->AddEventSink(GetSingleton());
			logger::info("Handler Installed: Reference Attach");
		}
	}
};

// Backup in case the container does not get attached
class ObjectLoadedEventHandler : public RE::BSTEventSink<RE::TESObjectLoadedEvent> {
public:
	static ObjectLoadedEventHandler* GetSingleton() {
		static ObjectLoadedEventHandler singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override {	
		if (!event || !event->loaded || PlayerOwnedLocation() || !Settings::GetSingleton()->isDynamicEnabled())
			return RE::BSEventNotifyControl::kContinue;
		
		// Get the FormID of the object, and queue it for processing
		const auto formID = event->formID;
		SKSE::GetTaskInterface()->AddTask([formID]() {
			auto* ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(formID);
			if (!ref || ref->IsPlayerRef()) return;

			auto* base = ref->GetBaseObject();
			if (!base) return;

			if (ref->As<RE::Actor>() || base->Is(RE::FormType::Container))
				ProcessInventory(ref);
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register()
	{
		auto* eventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (eventSource) {
			eventSource->AddEventSink(GetSingleton());
			logger::info("Handler Installed: Object Loaded");
		}
	}
};

// =============================================================
// Break System Handler
// =============================================================
static void EquipObject(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, const RE::ObjectEquipParams& a_objectEquipParams) {
	if (a_actor && a_object && !a_objectEquipParams.forceEquip) {

		// If it has the broken keyword, stop it from equipping
		FoundEquipData eqD(a_object, a_objectEquipParams.extraDataList);
		if (eqD.IsBroken() && !Settings::GetSingleton()->ED_BreakDisabled) { 
			if (a_actor == Utility::GetSingleton()->GetPlayer()) {
				const char* objectName = a_object->GetName();
				auto msg = std::vformat(EDTranslation::Translate("Notification.Broken"), std::make_format_args(objectName));
				Utility::GetSingleton()->ShowNotification(msg, false, "VOCShoutImpactDisarm");
			}
			return;
		 }
	}
	return _EquipObject(a_manager, a_actor, a_object, a_objectEquipParams);
}

namespace Events {
	void RegisterSerialization() {
		auto* serialization = SKSE::GetSerializationInterface();
		if (!serialization) {
			logger::critical("Failed to acquire SKSE serialization interface");
			return;
		}

		serialization->SetUniqueID(kSerializationID);
		serialization->SetSaveCallback(SaveProcessedReferences);
		serialization->SetLoadCallback(LoadProcessedReferences);
		serialization->SetRevertCallback(RevertProcessedReferences);
		logger::info("Registered processed-reference serialization");
	}

	// inline static REL::Relocation<std::uintptr_t> On_Update_Hook{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xC26) };
	inline static REL::Relocation<std::uintptr_t> EquipObject_Hook{ REL::RelocationID(37938, 38894), REL::Relocate(0xE5, 0x170) };

	void Init(void) {
		// Event Overrides
		HitEventHandler::Register();
		InventoryResetHook::Install();
		BarterMenuHook::Install();
		ObjectLoadedEventHandler::Register();
		ReferenceAttachEventHandler::Register();
		PlayerGraphEventHook::Install();

		// OnEquip Hook
		auto& trampoline = SKSE::GetTrampoline();
		_EquipObject = trampoline.write_call<5>(EquipObject_Hook.address(), EquipObject);
		logger::info("Hook Installed: On Equip");

	}
}
