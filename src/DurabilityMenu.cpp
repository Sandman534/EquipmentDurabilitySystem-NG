#include "DurabilityMenu.h"
#include "FoundEquipData.h"
#include "Settings.h"
#include "Utility.h"
#include <vector>
#include "ItemData/ItemStack.h"

//===============================================
// Durability HUD
//===============================================
DurabilityMenu::DurabilityMenu() {
	// Base flags
	menuFlags.set(RE::UI_MENU_FLAGS::kRequiresUpdate);
	menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
	menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);

	// Set Menu Depth (Higher = drawn above others (vanilla pause menu is 3)
	depthPriority = 0;

	// Allow keyboard and mouse input
	inputContext = Context::kNone;

	// Load the uiMovie variable
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	if (scaleformManager->LoadMovie(this, uiMovie, MENU_NAME, RE::GFxMovieView::ScaleModeType::kNoScale)) {
		UpdatePosition();
	}
}

void DurabilityMenu::AdvanceMovie(float a_interval, std::uint32_t a_currentTime) {
	if(RE::UI::GetSingleton()->IsMenuOpen(RE::Console::MENU_NAME)) return;

	// Call base to handle input/messages
	RE::IMenu::AdvanceMovie(a_interval, a_currentTime);

	// update every 10th frame (~6 times/sec at 60 FPS)
	if (++frameCounter > 10)
		frameCounter = 0;
	else
		return;

	// Beast Check
	if (Utility::GetSingleton()->PlayerIsBeast())
		return;

	// Hotkey Timer
	auto settings = Settings::GetSingleton();
	if (settings->ED_Widget_Display == 3 && uiMovie->GetVisible() && hotkeyActivated && settings->ED_Widget_ToggleDuration > 0.0) {
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count();
		if (elapsed >= settings->ED_Widget_ToggleDuration) {
			hotkeyActivated = false;
			HideMenu();
		}
	}

	// Update the menu data
	UpdateItemData();
}

void DurabilityMenu::MenuState() {
	auto settings = Settings::GetSingleton();
	bool shouldShow = false;
	switch (settings->ED_Widget_Display) {
		case 1:
			shouldShow = true;
			break;
		case 2:
			shouldShow = sheathActivated;
			break;
		case 3:
			shouldShow = hotkeyActivated;
			break;
	}

	if (shouldShow)
		ShowMenu();
	else if (!shouldShow)
		HideMenu();
	if (hotkeyActivated)
		startTime = std::chrono::steady_clock::now();
}

void DurabilityMenu::ShowMenu() {
	auto hud = RE::UI::GetSingleton()->GetMenu("DurabilityMenu");
	if (hud) {
		// Fully hide it for the engine
		RE::UIMessage msg(MENU_NAME,RE::UI_MESSAGE_TYPE::kShow);
		hud->ProcessMessage(msg);

		// Hide the movie visually
		if (hud->uiMovie)
			hud->uiMovie->SetVisible(true);
	}
}

void DurabilityMenu::HideMenu() {
	auto hud = RE::UI::GetSingleton()->GetMenu(MENU_NAME);
	if (hud) {
		// Fully hide it for the engine
		RE::UIMessage msg(MENU_NAME,RE::UI_MESSAGE_TYPE::kHide);
		hud->ProcessMessage(msg); 

		// Hide the movie visually
		if (hud->uiMovie)
			hud->uiMovie->SetVisible(false);
	}
}

void DurabilityMenu::UpdatePosition() {
	if (uiMovie) {
		auto* setting = Settings::GetSingleton();
		RE::GFxValue args[8];
		args[0].SetNumber(setting->ED_Widget_PosX / 100.0);
		args[1].SetNumber(setting->ED_Widget_PosY / 100.0);
		args[2].SetNumber(setting->ED_Widget_Scale);
		args[3].SetBoolean(setting->ED_Widget_Reverse);
		args[4].SetBoolean(setting->ED_Widget_ShowShout);
		args[5].SetBoolean(setting->ED_Widget_ShowArmorName);
		args[6].SetBoolean(setting->ED_Widget_ShowWeaponName);
		args[7].SetBoolean(setting->ED_Widget_ShowHealth);

		uiMovie->Invoke("_root.widget.Setup", nullptr, args, 8);
	}
}

void DurabilityMenu::UpdateItemData() {
	auto utility = Utility::GetSingleton();
	if (!uiMovie || !uiMovie->GetVisible()) return;

	auto* setting = Settings::GetSingleton();
	std::vector<FoundEquipData> eqDs;
	eqDs.reserve(7);

	// Look for player inventory changes or visibile. Equipment will default to blank on !visible
	RE::InventoryChanges* exChanges = utility->GetPlayer()->GetInventoryChanges();
	if (!exChanges) {
		FoundEquipData eqD;
		for (int i = 0; i < 7; i++)
			eqDs.push_back(eqD);

	// Order of Equipment: Head, Body, Hands, Feet, Left Hand, Right Hand, Power
	} else {
		// Add Equipment in order
		FoundEquipData eqD_head = FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHead);
		if (eqD_head.baseForm)
			eqDs.push_back(eqD_head);
		else
			eqDs.push_back(FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHair));

		eqDs.push_back(FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kBody));
		eqDs.push_back(FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHands));
		eqDs.push_back(FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kFeet));

		// Add Left Hand
		RE::TESForm* left = utility->GetPlayer()->GetEquippedObject(true);
		if (left && left->IsWeapon())
			eqDs.push_back(FindEquippedWeapon(exChanges, left, true));
		else if (left && left->IsArmor() && left->As<RE::TESObjectARMO>()->HasPartOf(RE::BGSBipedObjectForm::BipedObjectSlot::kShield))
			eqDs.push_back(FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kShield));
		else if (left)
			eqDs.push_back(FoundEquipData(left));
		else
			eqDs.push_back(FoundEquipData(utility->Unarmed));

		// Add Right Hand
		RE::TESForm* right = utility->GetPlayer()->GetEquippedObject(false);
		if (right && right->IsWeapon())
			eqDs.push_back(FindEquippedWeapon(exChanges, right, false));
		else if (right)
			eqDs.push_back(FoundEquipData(right));
		else
			eqDs.push_back(FoundEquipData(utility->Unarmed));

		// Add Power
		eqDs.push_back(FoundEquipData(utility->GetPlayer()->GetActorRuntimeData().selectedPower));
	}

	// Set up for 7 items, 5 values each + stack data
	int i = 0;
	RE::GFxValue args[35];

	// Loop through the found equipment data
	for (FoundEquipData& eqD : eqDs) {
		// Place holders before inserting into the menu arguments
		const char* iconLabel = "none";
		const char* iconText = "";
		double iconValue = -1;
		std::uint32_t iconCustomColor = setting->ED_Color_Unbreakable;
		RE::GFxValue stackData;
		stackData.SetNull();

		// Process the form
		if (eqD.baseForm && ((eqD.IsUnarmed() && setting->ED_Widget_ShowUnarmed) || !eqD.IsUnarmed())) {
			// Process the inventory through the stack
			const I4Data::Items::ItemStack stack{ &eqD };
			const auto& data = stack.GetData();

			// Icon Label
			iconLabel = data.iconLabel.value;

			// Get Name and Health Value
			if (eqD.baseForm->IsWeapon() || eqD.baseForm->IsArmor()) {
				// Generate Name
				eqD.CreateName();
				iconText = eqD.objectName.c_str();

				// Get Health Value and Color
				if (eqD.CanTemper()) {
					using std::max;
					iconValue = max(std::round((eqD.GetItemHealthForWidget() - 1.0f) * 1000.0), 0.0);
					if (setting->GetBreakChance(eqD.baseForm) != 0.0 && iconValue <= setting->ED_BreakThreshold && eqD.CanBreak())
						iconCustomColor = setting->ED_Color_Broken;
				}
			} else
				iconText = eqD.baseForm->GetName();

			// Create the stack data
			if (eqD.baseForm) stackData = stack.BuildDataObject(uiMovie.get());
		}

		// Add our values to the argument list
		args[i].SetString(iconLabel);
		args[i+1].SetString(iconText);
		args[i+2].SetNumber(iconValue);
		args[i+3].SetNumber(iconCustomColor);
		args[i+4] = stackData;

		i += 5;
	}

	// Send the new data to the menu for processing
	if (uiMovie) uiMovie->Invoke("_root.widget.UpdateMenu", nullptr, args, 35);
}

//===============================================
// Event Sinks
//===============================================
class MenuListener : RE::BSTEventSink<RE::MenuOpenCloseEvent> {
public:
	static MenuListener* GetSingleton() {
		static MenuListener singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
		// If the menu is the Durability HUD or Quick Loot, move on
		if (!a_event) return RE::BSEventNotifyControl::kContinue;
		if (a_event->menuName == DurabilityMenu::MENU_NAME || a_event->menuName == "LootMenu") return RE::BSEventNotifyControl::kContinue;
			
		auto durability = DurabilityMenu::GetSingleton();
		if (!durability) return RE::BSEventNotifyControl::kContinue;

		// Depending on whats going on we will display or hide the menu
		if (a_event->menuName == RE::LoadingMenu::MENU_NAME && !a_event->opening)
			RE::UIMessageQueue::GetSingleton()->AddMessage(DurabilityMenu::MENU_NAME,RE::UI_MESSAGE_TYPE::kShow,nullptr);
		else if (Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton()) && a_event->opening)
			durability->HideMenu();
		else if (!Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton()) && !a_event->opening)
			durability->MenuState();

		// Continue
        return RE::BSEventNotifyControl::kContinue;
    }

	static void Register() {
		RE::UI *eventHolder = RE::UI::GetSingleton();
        eventHolder->AddEventSink(MenuListener::GetSingleton());
		logger::info("Handler Installed: Menu OpenClose");
	}
};

class MenuInputHandler : public RE::BSTEventSink<RE::InputEvent*> {
	public:
    static MenuInputHandler* GetSingleton() {
		static MenuInputHandler singleton;
		return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,RE::BSTEventSource<RE::InputEvent*>* a_eventSource) {
		if (a_event) {
			// If a block menu is open, do not process
			if (Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton())) return RE::BSEventNotifyControl::kContinue;

			const auto controlMap = RE::ControlMap::GetSingleton();
			const auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			const auto playerControls = RE::PlayerControls::GetSingleton();

			// If we dont have any of these, return
			if (!controlMap || !playerCharacter || !playerControls) return RE::BSEventNotifyControl::kContinue;

			// Was a key pressed that matches our menu hotkey
			for (auto event = *a_event; event; event = event->next) {
				if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
					const auto button = static_cast<RE::ButtonEvent*>(event);
					if (!button || (button->IsPressed() && !button->IsDown())) continue;

					auto device = button->device.get();
					auto scan_code = HelperFunctions::AdjustScanCodes(device, button->GetIDCode());

					if ((device == RE::INPUT_DEVICE::kKeyboard || device == RE::INPUT_DEVICE::kGamepad) && !button->IsUp()) {
						auto durability = DurabilityMenu::GetSingleton();
						if (durability && scan_code == Settings::GetSingleton()->ED_Widget_ToggleKeyCode) {
							if (!durability->hotkeyActivated)
								durability->hotkeyActivated = true;
							else
								durability->hotkeyActivated = false;
							durability->MenuState();	
						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register() {
        RE::BSInputDeviceManager* inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
        inputDeviceManager->AddEventSink(MenuInputHandler::GetSingleton());
		logger::info("Handler Installed: Input");
    }
};

RE::BSEventNotifyControl PlayerGraphEventHook::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_sink, RE::BSAnimationGraphEvent *a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource) {
	if (a_event->tag == "weaponDraw" || a_event->tag == "weaponSheathe") {
		if (auto durability = DurabilityMenu::GetSingleton()) {
			if (a_event->tag == "weaponDraw") durability->sheathActivated = true;
			else if (a_event->tag == "weaponSheathe") durability->sheathActivated = false;
			durability->MenuState();
		}
	}
    return _ProcessEvent(a_sink, a_event, a_eventSource);
}

namespace Menu {
	void Init(void) {
		MenuInputHandler::Register();
		MenuListener::Register();
	}

	void MenuInit(void) {
		auto ui = RE::UI::GetSingleton();
		if (!ui) return;
		RE::UI::GetSingleton()->Register(DurabilityMenu::MENU_NAME, &DurabilityMenu::Create);
		RE::UIMessageQueue::GetSingleton()->AddMessage(DurabilityMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
		logger::info("Loaded: HUD");
	}
}