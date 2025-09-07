#pragma once

// =============================================================
// Dynamic Scan Throttle
// =============================================================
constexpr size_t kMaxProcessedContainers = 5000;
constexpr size_t kMaxProcessedNPCs = 2000;

// =============================================================
// On Update
// =============================================================
static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());
static float lastTime;
static std::int32_t OnUpdate();
static void EquipObject(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, const RE::ObjectEquipParams& a_objectEquipParams);

static REL::Relocation<decltype(OnUpdate)> _OnUpdate;
static REL::Relocation<decltype(EquipObject)> _EquipObject;

namespace Events {
	void Init(void);
}
