#pragma once

#include <chrono>
#include "FoundEquipData.h"

namespace Menu {
	void Init(void);
	void MenuInit(void);
	void Reopen(void);
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
		auto* ui = RE::UI::GetSingleton();
		if (!ui)
			return nullptr;
		return static_cast<DurabilityMenu*>(ui->GetMenu(MENU_NAME).get());
    }

    static RE::IMenu* Create() {
		return new DurabilityMenu();
    }

	void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
	void PostCreate() override;
	RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

	FoundEquipData GetHand(RE::InventoryChanges* a_changes, bool a_lefthand);
	void MenuState();
	void RestoreMenu();
	void ToggleHotkey();
	void ShowMenu();
	void HideMenu();
	void UpdatePosition();
};
