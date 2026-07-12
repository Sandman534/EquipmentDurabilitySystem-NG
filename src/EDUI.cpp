#include "EDUI.h"

void EDUI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }

	// Render Menu
    SKSEMenuFramework::SetSection("Equipment Durability NG");
	SKSEMenuFramework::AddSectionItem("Degradation", RenderRates);
	SKSEMenuFramework::AddSectionItem("Mitigation", RenderMaterial);
    SKSEMenuFramework::AddSectionItem("Dynamic Systems", RenderDynamic);
	SKSEMenuFramework::AddSectionItem("Durability HUD", RenderHUD);
	SKSEMenuFramework::AddSectionItem("Temper Names", RenderTemper);
}

void __stdcall EDUI::RenderRates() {
	Settings* Settings = Settings::GetSingleton();
	bool changeRateOption = false;

	if (CollapsingHeader("General Options", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Disable Equipment Degradation", &Settings->ED_DegradationDisabled))
			changeRateOption = true;
		if (Checkbox("Affect Player", &Settings->ED_AffectPlayer))
			changeRateOption = true;
		if (Checkbox("Affect Followers", &Settings->ED_AffectFollower))
			changeRateOption = true;
		if (Checkbox("Affect NPCs", &Settings->ED_AffectNPC))
			changeRateOption = true;
	}

	if (CollapsingHeader("Break Options", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Disable Equipment Breaking", &Settings->ED_BreakDisabled))
			changeRateOption = true;
		if (Checkbox("Do Not Break 'Disallowed Enchanted' Items", &Settings->ED_NoBreakNoEnchant))
			changeRateOption = true;
		if (Checkbox("Higher Durability Reduces Break Chance", &Settings->ED_IncreasedDurability))
			changeRateOption = true;
		Text("Break Chance Start");
		if (SliderInt("##HealthThreshold", &Settings->ED_BreakThreshold, 0, 700))
			changeRateOption = true;
	}

	if (CollapsingHeader("Durability Rates", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (BeginTable("##SystemRates", 3, ImGuiTableFlags_BordersV )) {
			TableSetupColumn("Category");
			TableSetupColumn("Degradation Rate");
			TableSetupColumn("Break Chance");
			TableHeadersRow();

			// Degradation Rates
			if (DegradeEntry("Swords", Settings->ED_Degrade_Sword, Settings->ED_Break_Sword))
				changeRateOption = true;
			if (DegradeEntry("Dagger", Settings->ED_Degrade_Dagger, Settings->ED_Break_Dagger))
				changeRateOption = true;
			if (DegradeEntry("War Axes", Settings->ED_Degrade_WarAxe, Settings->ED_Break_WarAxe))
				changeRateOption = true;
			if (DegradeEntry("Maces", Settings->ED_Degrade_Mace, Settings->ED_Break_Mace))
				changeRateOption = true;
			if (DegradeEntry("Great Swords", Settings->ED_Degrade_GreatSword, Settings->ED_Break_GreatSword))
				changeRateOption = true;
			if (DegradeEntry("Hammers", Settings->ED_Degrade_Warhammer, Settings->ED_Break_Warhammer))
				changeRateOption = true;
			if (DegradeEntry("Battle Axe", Settings->ED_Degrade_BattleAxe, Settings->ED_Break_BattleAxe))
				changeRateOption = true;
			if (DegradeEntry("Bows", Settings->ED_Degrade_Bow, Settings->ED_Break_Bow))
				changeRateOption = true;
			if (DegradeEntry("Crossbows", Settings->ED_Degrade_CrossBow, Settings->ED_Break_CrossBow))
				changeRateOption = true;

			TableNextRow();
			TableSetColumnIndex(0); Text("");
			TableSetColumnIndex(1); Text("");
			TableSetColumnIndex(2); Text("");

			if (DegradeEntry("Light Armor", Settings->ED_Degrade_LightArmor, Settings->ED_Break_LightArmor))
				changeRateOption = true;
			if (DegradeEntry("Heavy Armor", Settings->ED_Degrade_HeavyArmor, Settings->ED_Break_HeavyArmor))
				changeRateOption = true;
			if (DegradeEntry("Clothing", Settings->ED_Degrade_Clothing, Settings->ED_Break_Clothing))
				changeRateOption = true;
			if (DegradeEntry("Other Armor", Settings->ED_Degrade_Armor, Settings->ED_Break_Armor))
				changeRateOption = true;

			TableNextRow();
			TableSetColumnIndex(0); Text("");
			TableSetColumnIndex(1); Text("");
			TableSetColumnIndex(2); Text("");

			if (MultiplierEntry("Power Attack Multipler", Settings->ED_Degrade_PowerAttack, Settings->ED_Break_PowerAttack))
				changeRateOption = true;
			if (MultiplierEntry("Follower Multipler", Settings->ED_Degrade_FollowerMulti, Settings->ED_Break_FollowerMulti))
				changeRateOption = true;
			if (MultiplierEntry("NPC Multipler", Settings->ED_Degrade_NPCMulti, Settings->ED_Break_NPCMulti))
				changeRateOption = true;

			EndTable();
		}
	}

	if (changeRateOption)
		Settings->SaveINI();
}

void __stdcall EDUI::RenderMaterial() {
	Settings* Settings = Settings::GetSingleton();
	bool changeMaterialOption = false;

	if (CollapsingHeader("Skill Mitigation", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Skill Mitigation", &Settings->ED_Skill_Enabled))
			changeMaterialOption = true;
		Text("Skill Mitigation Factor");
		if (SliderInt("##HleathThreshold", &Settings->ED_Skill_Rate, 0, 100, "%d%%"))
			changeMaterialOption = true;
	}

	if (CollapsingHeader("Material Mitigation", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Enable Material Multiplier", &Settings->ED_Material_Multiplier))
			changeMaterialOption = true;

		if (BeginTable("##MaterialRates", 2)) {
			TableSetupColumn("Material Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			TableSetupColumn("Mitigation Rate");
			TableHeadersRow();

			if (EDUI::MaterialEntry("Fur", Settings->ED_Fur))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Leather", Settings->ED_Leather))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Iron", Settings->ED_Iron))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Steel", Settings->ED_Steel))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Silver", Settings->ED_Silver))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Elven", Settings->ED_Elven))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Bonemold", Settings->ED_Bonemold))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Falmer", Settings->ED_Falmer))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Chitin", Settings->ED_Chitin))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Dwarven", Settings->ED_Dwarven))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Glass", Settings->ED_Glass))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Orcish", Settings->ED_Orcish))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Stalhrim", Settings->ED_Stalhrim))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Ebony", Settings->ED_Ebony))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Dragon", Settings->ED_Dragon))
				changeMaterialOption = true;
			if (EDUI::MaterialEntry("Daedric", Settings->ED_Daedric))
				changeMaterialOption = true;
			EndTable();
		}
	}

	if (changeMaterialOption)
		Settings->SaveINI();
}

void __stdcall EDUI::RenderDynamic() {
	Settings* Settings = Settings::GetSingleton();
	bool dynamicChanged = false;

	if (CollapsingHeader("Dynamic Tempering", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Tempering Enabled", &Settings->ED_Temper_Enabled))
			dynamicChanged = true;

		Text("Base Chance");
		if (SliderInt("##Temper_BaseChance", &Settings->ED_Temper_Chance, 0, 100, "%d%%"))
			dynamicChanged = true;
			
		Text("Vendor Inventory Chance");
		if (SliderInt("##Temper_VendorInventoryChance", &Settings->ED_Temper_VendorChance, 0, 100, "%d%%"))
			dynamicChanged = true;

		Text("Boss Inventory Chance");
		if (SliderInt("##Temper_BossInventoryChance", &Settings->ED_Temper_BossChance, 0, 100, "%d%%"))
			dynamicChanged = true;
	}

	if (CollapsingHeader("Dynamic Enchanting", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (Checkbox("Enchanting Enabled", &Settings->ED_Enchant_Enabled))
			dynamicChanged = true;

		Text("Base Chance");
		if (SliderInt("##Enchanting_BaseChancee", &Settings->ED_Enchant_Chance, 0, 100, "%d%%"))
			dynamicChanged = true;

		Text("Vendor Inventory Chance");
		if (SliderInt("##Enchanting_VendorInventoryChance", &Settings->ED_Enchant_VendorChance, 0, 100, "%d%%"))
			dynamicChanged = true;

		Text("Boss Inventory Chance");
		if (SliderInt("##Enchanting_BossInventoryChance", &Settings->ED_Enchant_BossChance, 0, 100, "%d%%"))
			dynamicChanged = true;
	}

	if (dynamicChanged)
		Settings->SaveINI();
}

void __stdcall EDUI::RenderHUD() {
	Settings* Settings = Settings::GetSingleton();
	bool hudChanged = false;

	if (CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Display option for the Menu
		if (BeginCombo("Displayed", displayOptions[Settings->ED_Widget_Display].c_str())) {
			for (int i = 0; i < displayOptions.size(); i++) {
				bool is_selected = (Settings->ED_Widget_Display == i);
				if (Selectable(displayOptions[i].c_str(), is_selected)) {
					Settings->ED_Widget_Display = i;
					hudChanged = true;
					auto ui = RE::UI::GetSingleton();
					if (!ui) return; 

					auto menuBase = ui->GetMenu("DurabilityMenu");
					if (!menuBase) return;

					auto durabilityMenu = static_cast<DurabilityMenu*>(menuBase.get());
					if (!durabilityMenu) return;

					// Call your function
					durabilityMenu->MenuState();
				}
			}
			EndCombo();
		}

		// X/Y Position and Scale
		if (BeginTable("##Hud_Position", 2, ImGuiTableFlags_NoBordersInBody)) {
			TableSetupColumn("X Position");
			TableSetupColumn("Y Position");
			TableHeadersRow();

			TableNextRow();
			TableSetColumnIndex(0); 
			if (SliderInt("##Widget_XPosition", &Settings->ED_Widget_PosX, 0, 100, "%d%%"))
				hudChanged = true;
			TableSetColumnIndex(1); 
			if (SliderInt("##Widget_YPosition", &Settings->ED_Widget_PosY, 0, 100, "%d%%"))
				hudChanged = true;
			
			EndTable();
		}
		Text("Scale");
		if (SliderInt("##Widget_Scale", &Settings->ED_Widget_Scale, 0, 200, "%d%%"))
			hudChanged = true;

		// Unbreakable and Breakable Colors
		ImVec4 color_unbreak(
			((Settings->ED_Color_Unbreakable >> 16) & 0xFF) / 255.0f, // R
			((Settings->ED_Color_Unbreakable >> 8) & 0xFF) / 255.0f,  // G
			( Settings->ED_Color_Unbreakable & 0xFF) / 255.0f,        // B
			1.0f                                                 	  // A
		);
		if (ColorEdit4("Unbreakable Color", &color_unbreak.x, ImGuiColorEditFlags_NoAlpha)) {
			Settings->ED_Color_Unbreakable = (static_cast<uint32_t>(color_unbreak.x*255) << 16) | (static_cast<uint32_t>(color_unbreak.y*255) << 8) | (static_cast<uint32_t>(color_unbreak.z*255));
			hudChanged = true;
		}

		ImVec4 color_break(
			((Settings->ED_Color_Broken >> 16) & 0xFF) / 255.0f, // R
			((Settings->ED_Color_Broken >> 8) & 0xFF) / 255.0f,  // G
			( Settings->ED_Color_Broken & 0xFF) / 255.0f,        // B
			1.0f                                                 // A
		);
		if (ColorEdit4("Breakable Color", &color_break.x, ImGuiColorEditFlags_NoAlpha)) {
			Settings->ED_Color_Broken = (static_cast<uint32_t>(color_break.x*255) << 16) | (static_cast<uint32_t>(color_break.y*255) << 8) | (static_cast<uint32_t>(color_break.z*255));
			hudChanged = true;
		}

		// All of the Show/Hide Options
		if (BeginTable("##Show_Hide", 2, ImGuiTableFlags_NoBordersInBody)) {
			TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
			TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
			TableHeadersRow();

			TableSetColumnIndex(0); 
			if (Checkbox("Reverse Order", &Settings->ED_Widget_Reverse))
				hudChanged = true;
			TableSetColumnIndex(1); 
			if (Checkbox("Show Poison Name", &Settings->ED_Widget_ShowPoisonName))
				hudChanged = true;
			
			TableNextRow();
			TableSetColumnIndex(0); 
			if (Checkbox("Show Health", &Settings->ED_Widget_ShowHealth))
				hudChanged = true;
			TableSetColumnIndex(1);
			if (Checkbox("Show Armor Name", &Settings->ED_Widget_ShowArmorName))
				hudChanged = true;

			TableNextRow();
			TableSetColumnIndex(0);
			if (Checkbox("Show Shout", &Settings->ED_Widget_ShowShout))
				hudChanged = true;
			TableSetColumnIndex(1);
			if (Checkbox("Show Weapon Name", &Settings->ED_Widget_ShowWeaponName))
				hudChanged = true;

			TableNextRow();
			TableSetColumnIndex(0);
			if (Checkbox("Show Unarmed", &Settings->ED_Widget_ShowUnarmed))
				hudChanged = true;
			TableSetColumnIndex(1);
			Text("");

			EndTable();
		}

		// Allows user to update HUD positioning
		if (Button("Update HUD")) {
			auto ui = RE::UI::GetSingleton();
			if (!ui) return; 

			auto menuBase = ui->GetMenu("DurabilityMenu");
			if (!menuBase) return;

			auto durabilityMenu = static_cast<DurabilityMenu*>(menuBase.get());
			if (!durabilityMenu) return;

			// Call your function
			durabilityMenu->UpdatePosition();
		}
	}

	// Hotkey Options
	if (CollapsingHeader("Hotkey Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		Text("HUD Hotkey");
		SameLine();
		int &hotkey = Settings->ED_Widget_ToggleKeyCode;

		// Show the button with current hotkey name
		if (Button(waitKey ? "Press a key..." : GetKeyName((ImGuiKey)HelperFunctions::IDCodeToImGuiKey(hotkey)))) {
			waitKey = true;
			hotkey = -1;
		}

		// If waiting, check for input
		if (waitKey) {
			for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
				if (IsKeyPressed((ImGuiKey)key)) {
					hotkey = HelperFunctions::ImGuiKeyToIDCode((ImGuiKey)key);
					waitKey = false;
					hudChanged = true;
					break;
				}
			}
		}

		// Show a Clear Button
		if (hotkey >= 0) {
			SameLine();
			if (Button("Reset")) {
				hotkey = -1;
				hudChanged = true;
			}
		}

		Text("  Display Duration (0 is persistent)");
		if (SliderInt("##Hotkey_Duration", &Settings->ED_Widget_ToggleDuration, 0, 30))
			hudChanged = true;
	}

	if (hudChanged)
		Settings->SaveINI();
}

void __stdcall EDUI::RenderTemper() {
	Settings* Settings = Settings::GetSingleton();
	bool temperChanged = false;

	// Name Settings
	std::string prefixText = Settings->ED_Names_Prefix;
	std::string postfixText = Settings->ED_Names_Postfix;
	std::string brokenText = Settings->ED_Names_Broken;

	// General Name Settings
	Columns(2);
	if (CollapsingHeader("Name Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Display option for the Menu
		Text("Naming Style");
		if (BeginCombo("##Naming_Style", styleOptions[GetStyleIndex(Settings->ED_Names_Style)].c_str())) {
			for (int i = 0; i < styleOptions.size(); i++) {
				bool is_selected = (GetStyleIndex(Settings->ED_Names_Style) == i);
				if (Selectable(styleOptions[i].c_str(), is_selected)) {
					Settings->ED_Names_Style = styleOptions[i];
					temperChanged = true;
				}
			}
			EndCombo();
		}

		// Pre and Post Entries
		if (BeginTable("##Pre_Post", 2, ImGuiTableFlags_NoBordersInBody)) {
			TableSetupColumn("Prefix");
			TableSetupColumn("Postfix");
			TableHeadersRow();

			TableNextRow();
			TableSetColumnIndex(0);

			if (CreateInputText("##Prefix_Text", prefixText)) {
				Settings->ED_Names_Prefix = prefixText;
				temperChanged = true;
			}
			TableSetColumnIndex(1);
			if (CreateInputText("##Postfix_Text", postfixText)) {
				Settings->ED_Names_Postfix = postfixText;
				temperChanged = true;
			}

			EndTable();
		}

		Text("Broken Text");
		if (CreateInputText("##Broken_Text", brokenText)) {
			Settings->ED_Names_Broken = brokenText;
			temperChanged = true;
		}
	}

	// Custom Name List
	NextColumn();
	if (CollapsingHeader("Custom Names", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0; i < Settings->CustomNames.size(); i++) {
			PushID(i);

			// Level Text
			Text("Level %02d:", i + 1);

			// Editable text field for each entry
			SameLine();
			SetNextItemWidth(250.0f); // width in pixels
			if (CreateInputText("##Entry", Settings->CustomNames[i]))
				temperChanged = true;

			// Up and Down Arrows
			SameLine();
			if (ArrowButton("##up", ImGuiDir_Up) && i > 0) {
				std::swap(Settings->CustomNames[i], Settings->CustomNames[i - 1]);
				temperChanged = true;
			}
			SameLine();
			if (ArrowButton("##down", ImGuiDir_Down) && i < Settings->CustomNames.size() - 1) {
				std::swap(Settings->CustomNames[i], Settings->CustomNames[i + 1]);
				temperChanged = true;
			}

			// Entry Removeal
			SameLine();
			if (Button("X")) {
				Settings->CustomNames.erase(Settings->CustomNames.begin() + i);
				PopID();
				temperChanged = true;
				break;  // break because we modified the vector
			}

			PopID();
		}

		// Add new entry button at the bottom
		if (Button("Add Entry")) {
			Settings->CustomNames.push_back("New Entry");
			temperChanged = true;
			SetKeyboardFocusHere();
		}
	}

	if (temperChanged)
		Settings->SaveINI();
}

int EDUI::GetStyleIndex(const std::string& value) {
    auto it = std::find(styleOptions.begin(), styleOptions.end(), value);
    if (it != styleOptions.end()) {
        return static_cast<int>(std::distance(styleOptions.begin(), it));
    }
    return 0; // Vanilla
}

bool EDUI::MaterialEntry(const char* label, int& value) {
	bool changeSlider = false;

	// Set the header
	TableNextRow();
	TableSetColumnIndex(0);
	Text(label);
	TableSetColumnIndex(1); 

	// Set the ID and create the slider
	std::string id = std::format("##Material_{}", label);
	if (SliderInt(id.c_str(), &value, 0, 100, "%d%%"))
		changeSlider = true;

	return changeSlider;
}

bool EDUI::DegradeEntry(const char* label, int& degradeValue, int& breakValue) {
	bool changeSlider = false;

	// Set Slider IDs
	std::string idd = std::format("##Degrade_{}", label);
	std::string idb = std::format("##Break_{}", label);

	// Set the header
	TableNextRow();
	TableSetColumnIndex(0);
	Text(label);
	
	// Degrade Slider
	TableSetColumnIndex(1); 
	if (SliderInt(idd.c_str(), &degradeValue, 0, 200, "%d%%"))
		changeSlider = true;

	// Break Slider
	TableSetColumnIndex(2); 
	if (SliderInt(idb.c_str(), &breakValue, 0, 100, "%d%%"))
		changeSlider = true;

	return changeSlider;
}

bool EDUI::MultiplierEntry(const char* label, int& value1, int& value2) {
	bool changeSlider = false;

	// Set Slider IDs
	std::string idd = std::format("##Degrade_{}", label);
	std::string idb = std::format("##Break_{}", label);

	// Set the header
	TableNextRow();
	TableSetColumnIndex(0);
	Text(label);
	
	// Degrade Slider
	TableSetColumnIndex(1); 
	if (SliderInt(idd.c_str(), &value1, 0, 200, "%d%%"))
		changeSlider = true;

	// Break Slider
	TableSetColumnIndex(2); 
	if (SliderInt(idb.c_str(), &value2, 0, 200, "%d%%"))
		changeSlider = true;

	return changeSlider;
}

bool EDUI::CreateInputText(const char* label, std::string& str, ImGuiInputTextFlags flags) {
    bool changed = false;

    struct UserData { std::string* str; bool* changed; };
    UserData user_data = { &str, &changed };

    auto callback = [](ImGuiInputTextCallbackData* data) -> int {
        UserData* ud = (UserData*)data->UserData;
        std::string* s = ud->str;

        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            s->resize(data->BufTextLen);
            data->Buf = (char*)s->c_str();
        }
        return 0;
    };

    // Only reserve some space if empty
    if (str.empty()) 
		str.reserve(64); // optional initial buffer

	str.resize(str.capacity());

    changed = InputText(
        label,
        str.data(),
        str.size() + 1,
        flags | ImGuiInputTextFlags_CallbackResize,
        callback,
        &user_data
    );

    return changed;
}
