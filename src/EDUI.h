#pragma once
#include "Settings.h"
#include "DurabilityMenu.h"

using namespace ImGuiMCP;

namespace EDUI {
    void Register();

    // Static Variables
	static bool waitKey = false;
    static std::vector<std::string> displayOptions = { "Never", "Always", "Weapon Drawn", "Hotkey" };
	static std::vector<std::string> styleOptions = { "Vanilla", "VanillaPlus", "PlusN", "Internal", "Custom", "RomanNumeral" };

    // Render Functions
    void __stdcall RenderRates();
	void __stdcall RenderMaterial();
	void __stdcall RenderDynamic();
	void __stdcall RenderHUD();
    void __stdcall RenderTemper();

    void __stdcall EventListener(SKSEMenuFramework::Model::EventType* eventType);

    // Additional Functions
	int GetStyleIndex(const std::string& value);
    bool MaterialEntry(const char* label, float& value);
    bool DegradeEntry(const char* label, float& degradeValue, int& breakValue);
    bool MultiplierEntry(const char* label, float& degradeValue, float& breakValue);
    void SliderEntryMaterial(float& value, const char* id);
	bool CreateInputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);
};