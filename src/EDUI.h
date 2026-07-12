#pragma once
#include "Settings.h"
#include "DurabilityMenu.h"

using namespace ImGuiMCP;

namespace EDUI {
    void Register();

    // Static Variables
	static bool waitKey = false;
    static std::vector<std::string> displayOptions = { "Never", "Always", "Weapon Drawn", "Hotkey" };
	static std::vector<std::string> styleOptions = { "Vanilla", "VanillaPlus", "PlusN", "Internal", "Custom", "RomanNumeral", "Health" };

    constexpr const char* translationsFolder = "Data/SKSE/Plugins/SKSEMenuFrameworkStrings.json";
    const char* defaultTranslation = "missing translation";
    static inline std::map<std::string, const char*> translations;

    // Translations
    void InstallTranslation();
    const const char* Translate(std::string key);

    // Render Functions
    void __stdcall RenderRates();
	void __stdcall RenderMaterial();
	void __stdcall RenderDynamic();
	void __stdcall RenderHUD();
    void __stdcall RenderTemper();

    // Additional Functions
	int GetStyleIndex(const std::string& value);
    bool MaterialEntry(const char* label, int& value);
    bool DegradeEntry(const char* label, int& degradeValue, int& breakValue);
    bool MultiplierEntry(const char* label, int& value1, int& value2);
    void SliderEntryMaterial(float& value, const char* id);
	bool CreateInputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);
};