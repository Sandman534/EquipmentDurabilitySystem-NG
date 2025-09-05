#pragma once

#include <chrono>
#include "FoundEquipData.h"

namespace Menu {
	void Init(void);
	void AnimationInit(void);
}

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
	
	void ShowMenu();
	void HideMenu();
	void UpdatePosition();
	void ResetTimer() { startTime = std::chrono::steady_clock::now(); };
	void HotkeyActivation(bool activated);
};