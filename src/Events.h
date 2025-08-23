#pragma once

static float& g_deltaTime = (*(float*)RELOCATION_ID(523660, 410199).address());
static float lastTime;

static std::int32_t OnUpdate();
static REL::Relocation<decltype(OnUpdate)> _OnUpdate;

namespace Events {
	void Init(void);
}
