#pragma once

class Utility {
private:
    const std::string_view pluginSkyrim = "Skyrim.esm";
    const std::string_view pluginUpdate = "Update.esm";
    const std::string_view pluginDawnguard = "Dawnguard.esm";
    const std::string_view pluginHeathfire = "HearthFires.esm";

    template <class T>
    T* LookupForm(RE::TESDataHandler* dataHandler, RE::FormID formID, std::string_view plugin, std::string_view name, bool required = true);

public:
    // Singleton
    static Utility* GetSingleton();

    // Keywords
    RE::BGSKeyword* keywordWarhammer;
    RE::BGSKeyword* keywordClothing;
    RE::BGSKeyword* keywordJewelry;
    RE::BGSKeyword* keywordMagicDisallow;

    // Races
    RE::TESRace* raceVampireLord;
    RE::TESRace* raceWerewolf;
    
    // Perks
    RE::BGSPerk* Undeath_LichPerk;

    // Locations
    RE::BGSLocationRefType* locationBoss;
    RE::BGSLocationRefType* locationBossContainer;

    // Factions
    RE::TESFaction* playerFaction;

    // Forms
	RE::TESForm* Unarmed;

    // Functions
    void LoadForms();
    RE::PlayerCharacter* GetPlayer();
    bool ActorIsNotBeast(RE::Actor* actor);
    void ShowNotification(std::string msg, bool messageBox = false, const char* a_soundToPlay = 0);
    bool MenuShouldHide(RE::UI* ui);
};
