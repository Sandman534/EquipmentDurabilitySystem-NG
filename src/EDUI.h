#pragma once
#include "Settings.h"

namespace EDUI {
    void Register();

    // Static Variables
	static bool waitKey = false;
    static std::vector<std::string> displayOptions = { "Never", "Always", "Weapon Drawn", "Hotkey" };

    // Render Functions
    void __stdcall RenderRates();
	void __stdcall RenderMaterial();
	void __stdcall RenderDynamic();
	void __stdcall RenderHUD();

    // Additional Functions
    void SliderEntryMaterial(float& value, const char* id);
};