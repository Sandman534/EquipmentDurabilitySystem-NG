#include "Utility.h"
#include "Settings.h"

// Singleton
Utility* Utility::GetSingleton() {
    static Utility UtilityFunctions;
    return &UtilityFunctions; 
}

template <class T>
T* Utility::LookupForm(RE::TESDataHandler* dataHandler, RE::FormID formID, std::string_view plugin, std::string_view name, bool required) {
    auto* form = dataHandler->LookupForm(formID, plugin);
    auto* result = form ? form->As<T>() : nullptr;

    if (!result) {
        if (required)
            logger::critical("Failed to load {}", name);
        else
            logger::warn("Failed to load {}", name);
    }

    return result;
}

RE::PlayerCharacter* Utility::GetPlayer() { 
    return RE::PlayerCharacter::GetSingleton(); 
}

void Utility::LoadForms() {
    const auto dataHandler = RE::TESDataHandler::GetSingleton();

    // Keywords
    keywordWarhammer = LookupForm<RE::BGSKeyword>(dataHandler, RE::FormID(0x06D930), pluginSkyrim, "Warhammer keyword");
    keywordClothing = LookupForm<RE::BGSKeyword>(dataHandler, RE::FormID(0x08F95B), pluginSkyrim, "Clothing keyword");
    keywordJewelry = LookupForm<RE::BGSKeyword>(dataHandler, RE::FormID(0x08F95A), pluginSkyrim, "Jewelry keyword");
    keywordMagicDisallow = LookupForm<RE::BGSKeyword>(dataHandler, RE::FormID(0x0C27BD), pluginSkyrim, "Magic Disallow keyword");    

    // Locations
    locationBoss = LookupForm<RE::BGSLocationRefType>(dataHandler, RE::FormID(0x0130F7), pluginSkyrim, "Boss location type");
    locationBossContainer = LookupForm<RE::BGSLocationRefType>(dataHandler, RE::FormID(0x0130F8), pluginSkyrim, "Boss container type");

    // Factions
    playerFaction = LookupForm<RE::TESFaction>(dataHandler, RE::FormID(0x000dB1), pluginSkyrim, "Player faction");

    // Forms
    Unarmed = LookupForm<RE::TESForm>(dataHandler, RE::FormID(0x0001F4), pluginSkyrim, "Unarmed form");

    // Races
    raceWerewolf = LookupForm<RE::TESRace>(dataHandler, RE::FormID(0x0CDD84), pluginSkyrim, "Werewolf race");
    raceVampireLord = LookupForm<RE::TESRace>(dataHandler, RE::FormID(0x00283A), pluginDawnguard, "Vampire Lord race", false);

    // Perks
    Undeath_LichPerk = LookupForm<RE::BGSPerk>(dataHandler, RE::FormID(0x3326D5), "Undeath.esp", "Undeath Lich perk", false);

    logger::info("Loaded: All Required Forms");
}

bool Utility::ActorIsNotBeast(RE::Actor* actor) {
    if (!actor) return false;

    bool isLich = false;
    if (Undeath_LichPerk) isLich = actor->HasPerk(Undeath_LichPerk);

    return actor->GetRace() != raceWerewolf && actor->GetRace() != raceVampireLord && !isLich;
}

void Utility::ShowNotification(std::string msg, bool messageBox, const char* a_soundToPlay) {
    if (messageBox) RE::DebugMessageBox(msg.c_str());
    else RE::DebugNotification(msg.c_str(), a_soundToPlay);
}

bool Utility::MenuShouldHide(RE::UI* ui) {
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
