#pragma once
#include "Settings.h"

class Utility {
private:
    const std::string_view pluginSkyrim = "Skyrim.esm";
    const std::string_view pluginUpdate = "Update.esm";
    const std::string_view pluginDawnguard = "Dawnguard.esm";
    const std::string_view pluginHeathfire = "HearthFires.esm";

public:
    // Keywords to identify items
    RE::BGSKeyword* keywordWarhammer;
    RE::BGSKeyword* keywordClothing;
    RE::BGSKeyword* keywordJewelry;
    RE::BGSKeyword* keywordMagicDisallow;

    // Determine the players race
    RE::TESRace* raceVampireLord;
    RE::TESRace* raceWerewolf;
    RE::BGSPerk* Undeath_LichPerk;

    // Boss Location References
    RE::BGSLocationRefType* locationBoss;
    RE::BGSLocationRefType* locationBossContainer;

    // Player Faction
    RE::TESFaction* playerFaction;

    // Unarmed Form
	RE::TESForm* Unarmed;

    // Weapon Health Star
	float MinHealth = 0.099f;
    float StepToMin = 0.0;

    // Singleton
    uintptr_t PlayerSingletonAddress;
    static Utility* GetSingleton() {
        static Utility playerStatus;
        return &playerStatus; 
    }

    void LoadForms() {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();

        // Keywords
        keywordWarhammer = dataHandler->LookupForm(RE::FormID(0x06D930), pluginSkyrim)->As<RE::BGSKeyword>();
        keywordClothing = dataHandler->LookupForm(RE::FormID(0x08F95B), pluginSkyrim)->As<RE::BGSKeyword>();
        keywordJewelry = dataHandler->LookupForm(RE::FormID(0x08F95A), pluginSkyrim)->As<RE::BGSKeyword>();
        keywordMagicDisallow = dataHandler->LookupForm(RE::FormID(0x0C27BD), pluginSkyrim)->As<RE::BGSKeyword>();

        // Locations
        locationBoss = dataHandler->LookupForm(RE::FormID(0x0130F7), pluginSkyrim)->As<RE::BGSLocationRefType>();
        locationBossContainer = dataHandler->LookupForm(RE::FormID(0x0130F8), pluginSkyrim)->As<RE::BGSLocationRefType>();

        // Player Faction
        playerFaction = dataHandler->LookupForm(RE::FormID(0x000dB1), pluginSkyrim)->As<RE::TESFaction>();

        // Unarmed Form
        Unarmed = dataHandler->LookupForm(RE::FormID(0x0001F4), pluginSkyrim)->As<RE::TESForm>();

        // Transformation Races
        raceWerewolf = dataHandler->LookupForm(RE::FormID(0x0CDD84), pluginSkyrim)->As<RE::TESRace>();
        raceVampireLord = dataHandler->LookupForm(RE::FormID(0x00283A), pluginDawnguard)->As<RE::TESRace>();

        // Undeath
        if (dataHandler->LookupLoadedModByName("Undeath.esp")) {
            if (auto lichPerk = dataHandler->LookupForm(RE::FormID(0x3326D5), "Undeath.esp"))
                Undeath_LichPerk = lichPerk->As<RE::BGSPerk>();
        }

        logger::info("Loaded: All Required Forms");
    }

    #pragma region Player Functions
    RE::PlayerCharacter* GetPlayer() { return RE::PlayerCharacter::GetSingleton(); }

    bool PlayerIsBeast() {
        bool isLich = false;
        if (Undeath_LichPerk) isLich = GetPlayer()->HasPerk(Undeath_LichPerk);

		return GetPlayer()->GetRace() == raceWerewolf || GetPlayer()->GetRace() == raceVampireLord || isLich;
    }
    #pragma endregion

    #pragma region Interface Functions
    void ShowNotification(std::string msg, bool messageBox = false, const char* a_soundToPlay = 0) {
        if (messageBox) RE::DebugMessageBox(msg.c_str());
        else RE::DebugNotification(msg.c_str(), a_soundToPlay);
    }

    bool MenuShouldHide(RE::UI* ui) {
        // Game is paused (menu/game paused)
        if (!ui) return true;
        if (ui->GameIsPaused()) return true;
        if (!ui->IsCursorHiddenWhenTopmost()) return true;
        if (!ui->IsShowingMenus()) return true;
        if (!ui->GetMenu<RE::HUDMenu>()) return true;
        if (ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME))  return true;
        //if (ui->IsMenuOpen("Console") || ui->IsMenuOpen("MessageBoxMenu") || ui->IsMenuOpen("RaceSexMenu")) return true;

        // Everything else is fine
        return false;
    }
    #pragma endregion

    #pragma region Equipment Health Values
    void ModifyHealth(float a_min, float a_step) { 
        MinHealth = a_min; 
        StepToMin = a_step;
    }

    float DefaultWidgetHealth() {
		return 1.0f + MinHealth + StepToMin;
    }

    float DefaultHealth() {
		return 1.0f + MinHealth;
    }

    float MinimumHealth() {
		return 0.999f - StepToMin;
    }

    float NormalizedHealth(float a_Health) {
		return (a_Health + StepToMin) - 0.999f;
    }
    #pragma endregion
};
