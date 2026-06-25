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

    // Singleton
    uintptr_t PlayerSingletonAddress;
    static Utility* GetSingleton() {
        static Utility playerStatus;
        return &playerStatus; 
    }

    void LoadForms() {
        const auto dataHandler = RE::TESDataHandler::GetSingleton();

        // Keywords
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x06D930), pluginSkyrim))
            keywordWarhammer = form->As<RE::BGSKeyword>();
        else
            logger::critical("Failed to load Warhammer keyword");

        if (auto* form = dataHandler->LookupForm(RE::FormID(0x08F95B), pluginSkyrim))
            keywordClothing = form->As<RE::BGSKeyword>();
        else
            logger::critical("Failed to load Clothing keyword");

        if (auto* form = dataHandler->LookupForm(RE::FormID(0x08F95A), pluginSkyrim))
            keywordJewelry = form->As<RE::BGSKeyword>();
        else
            logger::critical("Failed to load Jewelry keyword");

        if (auto* form = dataHandler->LookupForm(RE::FormID(0x0C27BD), pluginSkyrim))
            keywordMagicDisallow = form->As<RE::BGSKeyword>();
        else
            logger::critical("Failed to load Magic Disallow keyword");

        // Locations
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x0130F7), pluginSkyrim))
            locationBoss = form->As<RE::BGSLocationRefType>();
        else
            logger::critical("Failed to load Boss location type");

        if (auto* form = dataHandler->LookupForm(RE::FormID(0x0130F8), pluginSkyrim))
            locationBossContainer = form->As<RE::BGSLocationRefType>();
        else
            logger::critical("Failed to load Boss container type");

        // Player Faction
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x000dB1), pluginSkyrim))
            playerFaction = form->As<RE::TESFaction>();
        else
            logger::critical("Failed to load Player faction");

        // Unarmed Form
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x0001F4), pluginSkyrim))
            Unarmed = form->As<RE::TESForm>();
        else
            logger::critical("Failed to load Unarmed form");

        // Werewolf
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x0CDD84), pluginSkyrim))
            raceWerewolf = form->As<RE::TESRace>();
        else
            logger::critical("Failed to load Werewolf race");

        // Dawnguard - Vampire Lord
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x00283A), pluginDawnguard))
            raceVampireLord = form->As<RE::TESRace>();
        else
            logger::warn("Failed to load Vampire Lord race");

        // Undeath - Lich
        if (auto* form = dataHandler->LookupForm(RE::FormID(0x3326D5), "Undeath.esp"))
            Undeath_LichPerk = form->As<RE::BGSPerk>();
        else
            logger::warn("Failed to load Undeath Lich perk");

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
        if (ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME)) return true;

        // Everything else is fine
        return false;
    }
    #pragma endregion
};
