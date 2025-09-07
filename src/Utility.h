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

        logger::info("Loaded: All Required Forms");
    }


    // Public Functions
    RE::PlayerCharacter* GetPlayer() { return RE::PlayerCharacter::GetSingleton(); }

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

    void ShowNotification(std::string msg, bool messageBox = false, const char* a_soundToPlay = 0) {
        if (messageBox) RE::DebugMessageBox(msg.c_str());
        else RE::DebugNotification(msg.c_str(), a_soundToPlay);
    }
};
