#pragma once

static std::int32_t OnUpdate();
static REL::Relocation<decltype(OnUpdate)> _OnUpdate;

static void EquipObject(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, const RE::ObjectEquipParams& a_objectEquipParams);
static REL::Relocation<decltype(EquipObject)> _EquipObject;

namespace Events {
	void RegisterSerialization();
	void Init(void);
}
