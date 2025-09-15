#pragma once

#include <chrono>
#include "FoundEquipData.h"

namespace Menu {
	void Init(void);
	void MenuInit(void);
}

class PlayerGraphEventHook {
public:
    static void Install() {
        REL::Relocation<uintptr_t> AnimEventVtbl_PC{RE::VTABLE_PlayerCharacter[2]};
        _ProcessEvent = AnimEventVtbl_PC.write_vfunc(0x1, ProcessEvent);
        SKSE::log::info("Hook Installed: Player Graph Event");
    }

private:
    static RE::BSEventNotifyControl ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_sink, RE::BSAnimationGraphEvent *a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource);
    static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;


};

class DurabilityMenu : public RE::IMenu {
private:
	std::chrono::steady_clock::time_point startTime;
	int frameCounter = 0;
	void UpdateItemData();

public:
	inline static constexpr const char* MENU_NAME = "DurabilityMenu";
	inline static bool hotkeyActivated = false;
	inline static bool sheathActivated = false;

	DurabilityMenu();

    static DurabilityMenu* GetSingleton() {
        static DurabilityMenu singleton;
        return std::addressof(singleton);
    }

    static RE::IMenu* Create() {
        return GetSingleton();
    }

	void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;

	void MenuState();
	void ShowMenu();
	void HideMenu();
	void UpdatePosition();
	void ResetTimer() { startTime = std::chrono::steady_clock::now(); };
};