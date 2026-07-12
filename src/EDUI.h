#pragma once
#include "Settings.h"
#include "DurabilityMenu.h"
#include <nlohmann/json.hpp>

using namespace ImGuiMCP;

namespace EDUI {
    void Register();

    // Static Variables
	inline bool waitKey = false;
    inline std::vector<std::string> displayOptions = { };
	inline std::vector<std::string> styleOptions = { };

    inline constexpr const char* translationsFolder = "Data\\SKSE\\Plugins\\EquipmentDurability\\EquipmentTranslation.json";
    inline const char* defaultTranslation = "missing translation";
    static inline std::map<std::string, const char*> translations;

    // Translations
    void InstallTranslation();
    const char* Translate(std::string key);

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