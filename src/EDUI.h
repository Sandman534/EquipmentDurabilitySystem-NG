#pragma once
#include "Settings.h"
#include "DurabilityMenu.h"

using namespace ImGuiMCP;

namespace EDUI {
    void Register();

    // Static Variables
	inline bool waitKey = false;
    inline std::vector<std::string> displayOptions = { };
    inline std::vector<std::string> layoutOptions = { };
	inline std::vector<std::string> styleOptions = { };

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
    void ShowHeaderText(const char* text);
    void ShowHeaderColumn(const char* text);
    bool UpdateMenu(bool value);
};
