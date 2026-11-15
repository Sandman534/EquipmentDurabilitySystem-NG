#include "DurabilityMenu.h"
#include "EDData.h"
#include "Settings.h"
#include "Utility.h"
#include <algorithm>
#include <vector>
#include "ItemData/ItemStack.h"

//===============================================
// Durability HUD
//===============================================
struct EquipmentEntry {
    WidgetSlot slot;
    FoundEquipData equipment;
};

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
		// The UI manager creates this always-open menu with a show message. Start
		// hidden so non-Always modes cannot render for a frame before PostCreate.
		uiMovie->SetVisible(false);
	}
}

void DurabilityMenu::PostCreate() {
	RE::IMenu::PostCreate();

	if (const auto* player = RE::PlayerCharacter::GetSingleton())
		sheathActivated = player->AsActorState()->IsWeaponDrawn();

	RestoreMenu();
}

RE::UI_MESSAGE_RESULTS DurabilityMenu::ProcessMessage(RE::UIMessage& a_message) {
	const auto result = RE::IMenu::ProcessMessage(a_message);
	if (a_message.type == RE::UI_MESSAGE_TYPE::kShow)
		RestoreMenu();
	return result;
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
	auto* utility = Utility::GetSingleton();
	if (!utility->ActorIsNotBeast(utility->GetPlayer()))
		return;

	// Hotkey Timer
	auto settings = Settings::GetSingleton();
	if (settings->ED_Widget_Display == 3 && uiMovie->GetVisible() && hotkeyActivated && settings->ED_Widget_ToggleDuration > 0) {
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
	bool shouldShow = !Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton());
	switch (settings->ED_Widget_Display) {
		case 1:
			break;
		case 2:
			shouldShow = shouldShow && sheathActivated;
			break;
		case 3:
			shouldShow = shouldShow && hotkeyActivated;
			break;
		default:
			shouldShow = false;
			break;
	}

	if (shouldShow)
		ShowMenu();
	else
		HideMenu();
}

void DurabilityMenu::RestoreMenu() {
	// Loading and cell transitions temporarily suppress the entire UI. Do not
	// include that transient global state here: prepare this movie according to
	// its display mode so it is already visible when Skyrim restores the HUD.
	auto* settings = Settings::GetSingleton();
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
		default:
			break;
	}

	if (shouldShow)
		ShowMenu();
	else
		HideMenu();
}

void DurabilityMenu::ToggleHotkey() {
	hotkeyActivated = !hotkeyActivated;
	if (hotkeyActivated)
		startTime = std::chrono::steady_clock::now();
	MenuState();
}

void DurabilityMenu::ShowMenu() {
	if (uiMovie)
		uiMovie->SetVisible(true);
}

void DurabilityMenu::HideMenu() {
	// Keep this kAlwaysOpen menu alive and only hide its own Scaleform movie.
	// Sending kHide through IMenu can alter the surrounding HUD/menu stack.
	if (uiMovie)
		uiMovie->SetVisible(false);
}

void DurabilityMenu::UpdatePosition() {
	if (uiMovie) {
		auto* setting = Settings::GetSingleton();
		RE::GFxValue args[5];
		args[0].SetNumber(setting->ED_Widget_PosX / 100.0);
		args[1].SetNumber(setting->ED_Widget_PosY / 100.0);
		args[2].SetNumber(setting->ED_Widget_Scale);
		args[3].SetNumber(setting->ED_Widget_Layout);
		args[4].SetBoolean(setting->ED_Widget_Reverse);
		uiMovie->Invoke("_root.widget.Setup", nullptr, args, 5);
	}
}

FoundEquipData DurabilityMenu::GetHand(RE::InventoryChanges* a_changes, bool a_lefthand) {
	// Initial Variables
	auto utility = Utility::GetSingleton();
	auto* setting = Settings::GetSingleton();
	FoundEquipData eqD;

	RE::TESForm* itemHand = utility->GetPlayer()->GetEquippedObject(a_lefthand);
	// If the item is a weapon, get it
	if (itemHand) {
		// Weapon
		if (itemHand->IsWeapon())
			eqD = FindEquippedWeapon(a_changes, utility->GetPlayer(), itemHand, a_lefthand);

		// Spell
		else if (itemHand->Is(RE::FormType::Spell)) {
			if (setting->ED_Widget_ShowSpells) eqD = FoundEquipData(itemHand);

		// Sheild
		} else if (itemHand->IsArmor())
			eqD = FindEquippedArmor(a_changes, RE::BGSBipedObjectForm::BipedObjectSlot::kShield);
		
		// Any other equipped item
		else
			eqD = FoundEquipData(itemHand);

	// If no item, show unarmed		
	} else
		eqD = FoundEquipData(utility->Unarmed);

	return eqD;
}

void DurabilityMenu::UpdateItemData() {
	auto utility = Utility::GetSingleton();
	if (!uiMovie || !uiMovie->GetVisible()) return;

	auto* setting = Settings::GetSingleton();
	std::vector<EquipmentEntry> eqData;

	// Order of Equipment: Head, Body, Hands, Feet, Left Hand, Right Hand, Power
	RE::InventoryChanges* exChanges = utility->GetPlayer()->GetInventoryChanges();
	if (exChanges) {
		// Add Equipment in order
		FoundEquipData eqD_head = FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHead);
		if (eqD_head.baseForm)
			eqData.push_back({WidgetSlot::Head, eqD_head});
		else
			eqData.push_back({WidgetSlot::Head, FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHair)});

		// Body, Hands and Feet
		eqData.push_back({WidgetSlot::Body, FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kBody)});
		eqData.push_back({WidgetSlot::Arms, FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kHands)});
		eqData.push_back({WidgetSlot::Foot, FindEquippedArmor(exChanges, RE::BGSBipedObjectForm::BipedObjectSlot::kFeet)});

		// Add Left and Right Hand
		eqData.push_back({WidgetSlot::LeftHand, GetHand(exChanges, true)});
		eqData.push_back({WidgetSlot::RightHand, GetHand(exChanges, false)});

		// Add Power
		eqData.push_back({WidgetSlot::Shout, FoundEquipData(utility->GetPlayer()->GetActorRuntimeData().selectedPower)});
	}

	// Sort by the configured slot order; unlisted slots retain their relative order at the end.
	const auto& order = setting->WidgetOrder;
	std::stable_sort(eqData.begin(), eqData.end(), [&order](const EquipmentEntry& a, const EquipmentEntry& b) {
		return std::find(order.begin(), order.end(), a.slot) <
			std::find(order.begin(), order.end(), b.slot);
	});

	// Send only visible entries; slot identity stays in each entry.
	RE::GFxValue entries;
	uiMovie->CreateArray(&entries);

	// Loop through the found equipment data
	for (EquipmentEntry& eqDataE : eqData) {

		// Set EquipData
		FoundEquipData& eqD = eqDataE.equipment;

		// Place holders before inserting into the menu arguments
		const char* iconLabel = "none";
		const char* iconText = "";
		double iconValue = -1;
		std::uint32_t iconCustomColor = setting->ED_Color_Unbreakable;
		RE::GFxValue stackData;
		stackData.SetNull();

		//If we are hiding a slot entirely, move on
		if ((!setting->ED_Widget_ShowHead && eqDataE.slot == WidgetSlot::Head) ||
			(!setting->ED_Widget_ShowBody && eqDataE.slot == WidgetSlot::Body) ||
			(!setting->ED_Widget_ShowHands && eqDataE.slot == WidgetSlot::Arms) ||
			(!setting->ED_Widget_ShowFeet && eqDataE.slot == WidgetSlot::Foot) ||
			(!setting->ED_Widget_ShowLeftHand && eqDataE.slot == WidgetSlot::LeftHand) ||
			(!setting->ED_Widget_ShowRightHand && eqDataE.slot == WidgetSlot::RightHand) ||
			(!setting->ED_Widget_ShowShout && eqDataE.slot == WidgetSlot::Shout)
		) {
			continue;
		}

		// Hidden entries must not consume a position in the HUD.
		if (!eqD.baseForm ||
			(!setting->ED_Widget_ShowUnarmed && eqD.IsUnarmed()) ||
			(setting->ED_Widget_ShowOnlyBreaking && !eqD.IsBelowBreakingThreshold()) ||
			(setting->ED_Widget_HideIndestructible && !eqD.CanTemper())) {
			continue;
		}

		// Process the form
		{
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
					iconValue = eqD.GetItemHealthForWidget();
					if (setting->GetBreakChance(eqD.baseForm) != 0.0 && eqD.IsBelowBreakingThreshold())
						iconCustomColor = setting->ED_Color_Broken;
				}
			} else
				iconText = eqD.baseForm->GetName();

			// Create the stack data
			if (eqD.baseForm) stackData = stack.BuildDataObject(uiMovie.get());
		}

		// Name Variable
		bool armor = eqDataE.slot == WidgetSlot::Head ||
			eqDataE.slot == WidgetSlot::Body ||
			eqDataE.slot == WidgetSlot::Arms ||
			eqDataE.slot == WidgetSlot::Foot;

		bool weapon = eqDataE.slot == WidgetSlot::LeftHand ||
			eqDataE.slot == WidgetSlot::RightHand;

		RE::GFxValue entry;
		uiMovie->CreateObject(&entry);
		entry.SetMember("slot", static_cast<int>(eqDataE.slot));
		entry.SetMember("icon", iconLabel);
		entry.SetMember("text", iconText);
		entry.SetMember("value", iconValue);
		entry.SetMember("color", iconCustomColor);
		entry.SetMember("data", stackData);
		entry.SetMember("showName", armor ? setting->ED_Widget_ShowArmorName : (weapon ? setting->ED_Widget_ShowWeaponName : true));
		entry.SetMember("showHealth", setting->ED_Widget_ShowHealth);
		entries.PushBack(entry);
	}

	uiMovie->Invoke("_root.widget.UpdateMenu", nullptr, &entries, 1);
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
			
		// Loading/cell transitions globally suppress the UI. Restore this movie's
		// desired state without treating the temporarily unavailable HUD as a
		// reason to persistently hide the movie.
		if (a_event->menuName == RE::LoadingMenu::MENU_NAME && !a_event->opening) {
			Menu::Reopen();
		}
		else if (a_event->menuName == RE::HUDMenu::MENU_NAME && a_event->opening) {
			SKSE::GetTaskInterface()->AddTask([] {
				if (auto* menu = DurabilityMenu::GetSingleton())
					menu->RestoreMenu();
				else
					Menu::Reopen();
			});
		}
		else if (auto* durability = DurabilityMenu::GetSingleton()) {
			if (Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton()) && a_event->opening)
				durability->HideMenu();
			else if (!Utility::GetSingleton()->MenuShouldHide(RE::UI::GetSingleton()) && !a_event->opening)
				durability->MenuState();
		}

		// Continue
        return RE::BSEventNotifyControl::kContinue;
    }

	static void Register() {
		RE::UI *eventHolder = RE::UI::GetSingleton();
        eventHolder->AddEventSink(MenuListener::GetSingleton());
		logger::info("Handler Installed: Menu OpenClose");
	}
};

class PlayerCellAttachListener : public RE::BSTEventSink<RE::TESCellAttachDetachEvent> {
public:
	static PlayerCellAttachListener* GetSingleton() {
		static PlayerCellAttachListener singleton;
		return &singleton;
	}

	RE::BSEventNotifyControl ProcessEvent(const RE::TESCellAttachDetachEvent* a_event,
		RE::BSTEventSource<RE::TESCellAttachDetachEvent>*) override {
		if (a_event && a_event->attached && a_event->reference && a_event->reference->IsPlayerRef()) {
			SKSE::GetTaskInterface()->AddTask([] { Menu::Reopen(); });
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	static void Register() {
		if (auto* source = RE::ScriptEventSourceHolder::GetSingleton()) {
			source->AddEventSink(GetSingleton());
			logger::info("Handler Installed: Player Cell Attach");
		}
	}
};

class MenuInputHandler : public RE::BSTEventSink<RE::InputEvent*> {
	public:
    static MenuInputHandler* GetSingleton() {
		static MenuInputHandler singleton;
		return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]]RE::BSTEventSource<RE::InputEvent*>* a_eventSource) {
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
					if (!button || !button->IsDown()) continue;

					auto device = button->device.get();
					auto scan_code = HelperFunctions::FixCode(device, button->GetIDCode());

					if (device == RE::INPUT_DEVICE::kKeyboard || device == RE::INPUT_DEVICE::kGamepad) {
						auto durability = DurabilityMenu::GetSingleton();
						auto settings = Settings::GetSingleton();
						if (durability && settings->ED_Widget_Display == 3 &&
							scan_code == settings->ED_Widget_ToggleKeyCode) {
							durability->ToggleHotkey();
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

namespace Menu {
	void Init(void) {
		MenuInputHandler::Register();
		MenuListener::Register();
		PlayerCellAttachListener::Register();
	}

	void Reopen(void) {
		auto* queue = RE::UIMessageQueue::GetSingleton();
		if (!queue)
			return;

		// A cell transition can leave the registered menu object alive while its
		// movie is no longer on the render stack. Recreate both the IMenu and SWF.
		queue->AddMessage(DurabilityMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
		queue->AddMessage(DurabilityMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
	}

	void MenuInit(void) {
		auto ui = RE::UI::GetSingleton();
		if (!ui) return;
		RE::UI::GetSingleton()->Register(DurabilityMenu::MENU_NAME, &DurabilityMenu::Create);
		RE::UIMessageQueue::GetSingleton()->AddMessage(DurabilityMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
		logger::info("Loaded: HUD");
	}
}
