#include "EDUI.h"
#include "DurabilityMenu.h"

void EDUI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {
        return;
    }

	// Render Menu
    SKSEMenuFramework::SetSection("Equipment Durability NG");
	SKSEMenuFramework::AddSectionItem("Degradation", RenderRates);
	SKSEMenuFramework::AddSectionItem("Material Multiplier", RenderMaterial);
    SKSEMenuFramework::AddSectionItem("Dynamic Systems", RenderDynamic);
	SKSEMenuFramework::AddSectionItem("Durability HUD", RenderHUD);
}

void __stdcall EDUI::RenderRates() {
	Settings* Settings = Settings::GetSingleton();

	if (ImGui::CollapsingHeader("General Options", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Disable Equipment Degradation", &Settings->ED_DegradationDisabled);
		ImGui::Checkbox("Disable Equipment Breaking", &Settings->ED_BreakDisabled);
		ImGui::Checkbox("Player Equipment Only", &Settings->ED_OnlyPlayer);
		ImGui::Checkbox("Do Not Break 'Disallowed Enchanted' Items", &Settings->ED_NoBreakNoEnchant);
		ImGui::Checkbox("Higher Durability Reduces Break Chance", &Settings->ED_IncreasedDurability);

		ImGui::Text("Break Chance Start");
		ImGui::SliderInt("##HleathThreshold", &Settings->ED_BreakThreshold, 0, 700);
	}

	if (ImGui::CollapsingHeader("Durability Rates", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("##SystemRates", 3, ImGuiTableFlags_BordersV )) {
			ImGui::TableSetupColumn("Category");
			ImGui::TableSetupColumn("Degradation Rate");
			ImGui::TableSetupColumn("Break Chance");
			ImGui::TableHeadersRow();

			// Degradation Rates
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Swords");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Sword", &Settings->ED_Degrade_Sword, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Sword", &Settings->ED_Break_Sword, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Dagger");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Dagger", &Settings->ED_Degrade_Dagger, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Dagger", &Settings->ED_Break_Dagger, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("War Axes");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_WarAxe", &Settings->ED_Degrade_WarAxe, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_WarAxe", &Settings->ED_Break_WarAxe, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Maces");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Mace", &Settings->ED_Degrade_Mace, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Mace", &Settings->ED_Break_Mace, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Great Swords");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_GreatSword", &Settings->ED_Degrade_GreatSword, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_GreatSword", &Settings->ED_Break_GreatSword, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Hammers");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Warhammer", &Settings->ED_Degrade_Warhammer, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Warhammer", &Settings->ED_Break_Warhammer, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Battle Axe");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_BattleAxe", &Settings->ED_Degrade_BattleAxe, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_BattleAxe", &Settings->ED_Break_BattleAxe, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Bows");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Bow", &Settings->ED_Degrade_Bow, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Bow", &Settings->ED_Break_Bow, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Crossbows");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Crossbow", &Settings->ED_Degrade_CrossBow, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Crossbow", &Settings->ED_Break_CrossBow, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("");
			ImGui::TableSetColumnIndex(1); ImGui::Text("");
			ImGui::TableSetColumnIndex(2); ImGui::Text("");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Light Armor");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_LightArmor", &Settings->ED_Degrade_LightArmor, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_LightArmor", &Settings->ED_Break_LightArmor, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Heavy Armor");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_HeavyArmor", &Settings->ED_Degrade_HeavyArmor, 0.0, 10.0, "%d%%");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_HeavyArmor", &Settings->ED_Break_HeavyArmor, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Clothing");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Clothing", &Settings->ED_Degrade_Clothing, 0.0, 10.0, "%d%%");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_Clothing", &Settings->ED_Break_Clothing, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Other Armor");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_OtherArmor", &Settings->ED_Degrade_Armor, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_OtherArmor", &Settings->ED_Break_Armor, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("");
			ImGui::TableSetColumnIndex(1); ImGui::Text("");
			ImGui::TableSetColumnIndex(2); ImGui::Text("");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Power Attack Multipler");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_PowerAttackMultipler", &Settings->ED_Degrade_PowerAttack, 0.0, 6.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderFloat("##Break_PowerAttackMultipler", &Settings->ED_Break_PowerAttack, 0.0, 6.0, "%.01f");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Follower Multipler");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_FollowerMultipler", &Settings->ED_Degrade_FollowerMulti, 0.0, 6.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderFloat("##Break_FollowerMultipler", &Settings->ED_Break_FollowerMulti, 0.0, 6.0, "%.01f");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("NPC Multipler");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_FollowerMultipler", &Settings->ED_Degrade_NPCMulti, 0.0, 6.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderFloat("##Break_FollowerMultipler", &Settings->ED_Break_NPCMulti, 0.0, 6.0, "%.01f");

			ImGui::EndTable();
		}
	}
}

void __stdcall EDUI::RenderMaterial() {
	Settings* Settings = Settings::GetSingleton();

	if (ImGui::CollapsingHeader("General Options", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Enable Material Multiplier", &Settings->ED_Material_Multiplier);
	}

	if (ImGui::CollapsingHeader("Material Rates", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginTable("##MaterialRates", 2)) {
			ImGui::TableSetupColumn("Material Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Durability Loss");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Fur");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Fur, "##Material_Fur");
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Leather");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Leather, "##Material_Leather");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Iron");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Iron, "##Material_Iron");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Steel");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Steel, "##Material_Steel");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Silver");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Silver, "##Material_Silver");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Elven");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Elven, "##Material_Elven");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Bonemold");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Bonemold, "##Material_Bonemold");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Falmer");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Falmer, "##Material_Falmer");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Chitin");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Chitin, "##Material_Chitin");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Dwarven");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Dwarven, "##Material_Dwarven");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Glass");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Glass, "##Material_Glass");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Orcish");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Orcish, "##Material_Orcish");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Stalhrim");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Stalhrim, "##Material_Stalhrim");
			 
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Ebony");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Ebony, "##Material_Ebony");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Dragon");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Dragon, "##Material_Dragon");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Daedric");
			ImGui::TableSetColumnIndex(1); EDUI::SliderEntryMaterial(Settings->ED_Daedric, "##Material_Daedric");
			
			ImGui::EndTable();
		}
	}
}

void __stdcall EDUI::RenderDynamic() {
	Settings* Settings = Settings::GetSingleton();

	if (ImGui::CollapsingHeader("Dynamic Tempering", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Tempering Enabled", &Settings->ED_Temper_Enabled);
		ImGui::Text("Base Chance");
		ImGui::SliderInt("##Temper_BaseChance", &Settings->ED_Temper_Chance, 0, 100, "%d%%");
		ImGui::Text("Vendor Inventory Chance");
		ImGui::SliderInt("##Temper_VendorInventoryChance", &Settings->ED_Temper_VendorChance, 0, 100, "%d%%");
		ImGui::Text("Boss Inventory Chance");
		ImGui::SliderInt("##Temper_BossInventoryChance", &Settings->ED_Temper_BossChance, 0, 100, "%d%%");
	}

	if (ImGui::CollapsingHeader("Dynamic Enchanting", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Enchanting Enabled", &Settings->ED_Enchant_Enabled);
		ImGui::Text("Base Chance");
		ImGui::SliderInt("##Enchanting_BaseChancee", &Settings->ED_Enchant_Chance, 0, 100, "%d%%");
		ImGui::Text("Vendor Inventory Chance");
		ImGui::SliderInt("##Enchanting_VendorInventoryChance", &Settings->ED_Enchant_VendorChance, 0, 100, "%d%%");
		ImGui::Text("Boss Inventory Chance");
		ImGui::SliderInt("##Enchanting_BossInventoryChance", &Settings->ED_Enchant_BossChance, 0, 100, "%d%%");
	}
}

void __stdcall EDUI::RenderHUD() {
	Settings* Settings = Settings::GetSingleton();

	if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Display option for the Menu
		if (ImGui::BeginCombo("Displayed", displayOptions[Settings->ED_Widget_Display].c_str())) {
			for (int i = 0; i < displayOptions.size(); i++) {
				bool is_selected = (Settings->ED_Widget_Display == i);
				if (ImGui::Selectable(displayOptions[i].c_str(), is_selected)) {
					Settings->ED_Widget_Display = i;
					auto ui = RE::UI::GetSingleton();
					if (!ui) return; 

					auto menuBase = ui->GetMenu("DurabilityMenu");
					if (!menuBase) return;

					auto durabilityMenu = static_cast<DurabilityMenu*>(menuBase.get());
					if (!durabilityMenu) return;

					// Call your function
					durabilityMenu->ShowMenu();
				}
			}
			ImGui::EndCombo();
		}

		// X/Y Position and Scale
		if (ImGui::BeginTable("##Hud_Position", 2, ImGuiTableFlags_NoBordersInBody)) {
			ImGui::TableSetupColumn("X Position");
			ImGui::TableSetupColumn("Y Position");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::SliderInt("##Widget_XPosition", &Settings->ED_Widget_PosX, 0, 100, "%d%%");
			ImGui::TableSetColumnIndex(1); ImGui::SliderInt("##Widget_YPosition", &Settings->ED_Widget_PosY, 0, 100, "%d%%");
			
			ImGui::EndTable();
		}
		ImGui::Text("Scale");
		ImGui::SliderInt("##Widget_Scale", &Settings->ED_Widget_Scale, 0, 200, "%d%%");

		// Unbreakable and Breakable Colors
		ImVec4 color_unbreak(
			((Settings->ED_Color_Unbreakable >> 16) & 0xFF) / 255.0f, // R
			((Settings->ED_Color_Unbreakable >> 8) & 0xFF) / 255.0f,  // G
			( Settings->ED_Color_Unbreakable & 0xFF) / 255.0f,        // B
			1.0f                                                 // A
		);
		if (ImGui::ColorEdit4("Unbreakable Color", &color_unbreak.x, ImGuiColorEditFlags_NoAlpha))
			Settings->ED_Color_Unbreakable = (static_cast<uint32_t>(color_unbreak.x*255) << 16) | (static_cast<uint32_t>(color_unbreak.y*255) << 8) | (static_cast<uint32_t>(color_unbreak.z*255));

		ImVec4 color_break(
			((Settings->ED_Color_Broken >> 16) & 0xFF) / 255.0f, // R
			((Settings->ED_Color_Broken >> 8) & 0xFF) / 255.0f,  // G
			( Settings->ED_Color_Broken & 0xFF) / 255.0f,        // B
			1.0f                                                 // A
		);
		if (ImGui::ColorEdit4("Breakable Color", &color_break.x, ImGuiColorEditFlags_NoAlpha)) {
			Settings->ED_Color_Broken = (static_cast<uint32_t>(color_break.x*255) << 16) | (static_cast<uint32_t>(color_break.y*255) << 8) | (static_cast<uint32_t>(color_break.z*255));
		}

		// All of the Show/Hide Options
		if (ImGui::BeginTable("##Show_Hide", 2, ImGuiTableFlags_NoBordersInBody)) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
			ImGui::TableHeadersRow();

			ImGui::TableSetColumnIndex(0); ImGui::Checkbox("Reverse Order", &Settings->ED_Widget_Reverse);
			ImGui::TableSetColumnIndex(1); ImGui::Checkbox("Show Poison Name", &Settings->ED_Widget_ShowPoisonName);
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Checkbox("Show Health", &Settings->ED_Widget_ShowHealth);
			ImGui::TableSetColumnIndex(1); ImGui::Checkbox("Show Armor Name", &Settings->ED_Widget_ShowArmorName);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Checkbox("Show Shout", &Settings->ED_Widget_ShowShout);
			ImGui::TableSetColumnIndex(1); ImGui::Checkbox("Show Weapon Name", &Settings->ED_Widget_ShowWeaponName);
			
			ImGui::EndTable();
		}

		// Allows user to update HUD positioning
		if (ImGui::Button("Update HUD")) {
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
	if (ImGui::CollapsingHeader("Hotkey Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("HUD Hotkey");
		ImGui::SameLine();
		int &hotkey = Settings->ED_Widget_ToggleKeyCode;

		// Show the button with current hotkey name
		if (ImGui::Button(waitKey ? "Press a key..." : ImGui::GetKeyName((ImGuiKey)IDCodeToImGuiKey(hotkey)))) {
			waitKey = true;
			hotkey = -1;
		}

		// If waiting, check for input
		if (waitKey) {
			for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
				if (ImGui::IsKeyPressed((ImGuiKey)key)) {
					hotkey = ImGuiKeyToIDCode((ImGuiKey)key);
					waitKey = false;
					break;
				}
			}
		}

		// Show a Clear Button
		if (hotkey >= 0) {
			ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				hotkey = -1;
			}
		}

		ImGui::Text("Display Duration (0 is persistent)");
		ImGui::SliderInt("##Hotkey_Duration", &Settings->ED_Widget_ToggleDuration, 0, 30);
	}
}

void EDUI::SliderEntryMaterial(float& value, const char* id) {
	int tempValue = value * 100;
	if (ImGui::SliderInt(id, &tempValue, 0, 200, "%d%%"))
		value = tempValue / 100.0f;
}

ImGuiKey EDUI::IDCodeToImGuiKey(int idCode)
{
	switch (idCode) {
	case RE::BSKeyboardDevice::Key::kA: return ImGuiKey_A;
	case RE::BSKeyboardDevice::Key::kB: return ImGuiKey_B;
	case RE::BSKeyboardDevice::Key::kC: return ImGuiKey_C;
	case RE::BSKeyboardDevice::Key::kD: return ImGuiKey_D;
	case RE::BSKeyboardDevice::Key::kE: return ImGuiKey_E;
	case RE::BSKeyboardDevice::Key::kF: return ImGuiKey_F;
	case RE::BSKeyboardDevice::Key::kG: return ImGuiKey_G;
	case RE::BSKeyboardDevice::Key::kH: return ImGuiKey_H;
	case RE::BSKeyboardDevice::Key::kI: return ImGuiKey_I;
	case RE::BSKeyboardDevice::Key::kJ: return ImGuiKey_J;
	case RE::BSKeyboardDevice::Key::kK: return ImGuiKey_K;
	case RE::BSKeyboardDevice::Key::kL: return ImGuiKey_L;
	case RE::BSKeyboardDevice::Key::kM: return ImGuiKey_M;
	case RE::BSKeyboardDevice::Key::kN: return ImGuiKey_N;
	case RE::BSKeyboardDevice::Key::kO: return ImGuiKey_O;
	case RE::BSKeyboardDevice::Key::kP: return ImGuiKey_P;
	case RE::BSKeyboardDevice::Key::kQ: return ImGuiKey_Q;
	case RE::BSKeyboardDevice::Key::kR: return ImGuiKey_R;
	case RE::BSKeyboardDevice::Key::kS: return ImGuiKey_S;
	case RE::BSKeyboardDevice::Key::kT: return ImGuiKey_T;
	case RE::BSKeyboardDevice::Key::kU: return ImGuiKey_U;
	case RE::BSKeyboardDevice::Key::kV: return ImGuiKey_V;
	case RE::BSKeyboardDevice::Key::kW: return ImGuiKey_W;
	case RE::BSKeyboardDevice::Key::kX: return ImGuiKey_X;
	case RE::BSKeyboardDevice::Key::kY: return ImGuiKey_Y;
	case RE::BSKeyboardDevice::Key::kZ: return ImGuiKey_Z;

	case RE::BSKeyboardDevice::Key::kNum0: return ImGuiKey_0;
	case RE::BSKeyboardDevice::Key::kNum1: return ImGuiKey_1;
	case RE::BSKeyboardDevice::Key::kNum2: return ImGuiKey_2;
	case RE::BSKeyboardDevice::Key::kNum3: return ImGuiKey_3;
	case RE::BSKeyboardDevice::Key::kNum4: return ImGuiKey_4;
	case RE::BSKeyboardDevice::Key::kNum5: return ImGuiKey_5;
	case RE::BSKeyboardDevice::Key::kNum6: return ImGuiKey_6;
	case RE::BSKeyboardDevice::Key::kNum7: return ImGuiKey_7;
	case RE::BSKeyboardDevice::Key::kNum8: return ImGuiKey_8;
	case RE::BSKeyboardDevice::Key::kNum9: return ImGuiKey_9;

	case RE::BSKeyboardDevice::Key::kF1:  return ImGuiKey_F1;
	case RE::BSKeyboardDevice::Key::kF2:  return ImGuiKey_F2;
	case RE::BSKeyboardDevice::Key::kF3:  return ImGuiKey_F3;
	case RE::BSKeyboardDevice::Key::kF4:  return ImGuiKey_F4;
	case RE::BSKeyboardDevice::Key::kF5:  return ImGuiKey_F5;
	case RE::BSKeyboardDevice::Key::kF6:  return ImGuiKey_F6;
	case RE::BSKeyboardDevice::Key::kF7:  return ImGuiKey_F7;
	case RE::BSKeyboardDevice::Key::kF8:  return ImGuiKey_F8;
	case RE::BSKeyboardDevice::Key::kF9:  return ImGuiKey_F9;
	case RE::BSKeyboardDevice::Key::kF10: return ImGuiKey_F10;
	case RE::BSKeyboardDevice::Key::kF11: return ImGuiKey_F11;
	case RE::BSKeyboardDevice::Key::kF12: return ImGuiKey_F12;

	case RE::BSKeyboardDevice::Key::kUp:	return ImGuiKey_UpArrow;
	case RE::BSKeyboardDevice::Key::kDown:	return ImGuiKey_DownArrow;
	case RE::BSKeyboardDevice::Key::kLeft:	return ImGuiKey_LeftArrow;
	case RE::BSKeyboardDevice::Key::kRight: return ImGuiKey_RightArrow;

	case RE::BSKeyboardDevice::Key::kHome:	   return ImGuiKey_Home;
	case RE::BSKeyboardDevice::Key::kEnd:	   return ImGuiKey_End;
	case RE::BSKeyboardDevice::Key::kInsert:   return ImGuiKey_Insert;
	case RE::BSKeyboardDevice::Key::kDelete:   return ImGuiKey_Delete;
	case RE::BSKeyboardDevice::Key::kPageUp:   return ImGuiKey_PageUp;
	case RE::BSKeyboardDevice::Key::kPageDown: return ImGuiKey_PageDown;

	case RE::BSKeyboardDevice::Key::kEscape:    return ImGuiKey_Escape;
	case RE::BSKeyboardDevice::Key::kSpacebar:  return ImGuiKey_Space;
	case RE::BSKeyboardDevice::Key::kEnter:	    return ImGuiKey_Enter;
	case RE::BSKeyboardDevice::Key::kTab:	    return ImGuiKey_Tab;
	case RE::BSKeyboardDevice::Key::kBackspace: return ImGuiKey_Backspace;

	// Implentation for Left/Right modifiers
	case RE::BSKeyboardDevice::Key::kLeftShift:	   return ImGuiKey_LeftShift;
	case RE::BSKeyboardDevice::Key::kRightShift:   return ImGuiKey_RightShift;
	case RE::BSKeyboardDevice::Key::kLeftControl:  return ImGuiKey_LeftCtrl;
	case RE::BSKeyboardDevice::Key::kRightControl: return ImGuiKey_RightCtrl;
	case RE::BSKeyboardDevice::Key::kLeftAlt:      return ImGuiKey_LeftAlt;
	case RE::BSKeyboardDevice::Key::kRightAlt:     return ImGuiKey_RightAlt;

	case RE::BSKeyboardDevice::Key::kEquals: return ImGuiKey_Equal;
	case RE::BSKeyboardDevice::Key::kMinus:  return ImGuiKey_Minus;

	case RE::BSKeyboardDevice::Key::kBracketLeft:  return ImGuiKey_LeftBracket;
	case RE::BSKeyboardDevice::Key::kBracketRight: return ImGuiKey_RightBracket;
	case RE::BSKeyboardDevice::Key::kBackslash:	   return ImGuiKey_Backslash;
	case RE::BSKeyboardDevice::Key::kSemicolon:    return ImGuiKey_Semicolon;
	case RE::BSKeyboardDevice::Key::kApostrophe:   return ImGuiKey_Apostrophe;
	case RE::BSKeyboardDevice::Key::kComma:		   return ImGuiKey_Comma;
	case RE::BSKeyboardDevice::Key::kPeriod:	   return ImGuiKey_Period;
	case RE::BSKeyboardDevice::Key::kSlash:		   return ImGuiKey_Slash;
	case RE::BSKeyboardDevice::Key::kTilde:		   return ImGuiKey_GraveAccent;

	case RE::BSKeyboardDevice::Key::kCapsLock:	 return ImGuiKey_CapsLock;
	case RE::BSKeyboardDevice::Key::kNumLock:	 return ImGuiKey_NumLock;
	case RE::BSKeyboardDevice::Key::kScrollLock: return ImGuiKey_ScrollLock;
	case RE::BSKeyboardDevice::Key::kPause:		 return ImGuiKey_Pause;

	case RE::BSKeyboardDevice::Key::kKP_0: return ImGuiKey_Keypad0;
	case RE::BSKeyboardDevice::Key::kKP_1: return ImGuiKey_Keypad1;
	case RE::BSKeyboardDevice::Key::kKP_2: return ImGuiKey_Keypad2;
	case RE::BSKeyboardDevice::Key::kKP_3: return ImGuiKey_Keypad3;
	case RE::BSKeyboardDevice::Key::kKP_4: return ImGuiKey_Keypad4;
	case RE::BSKeyboardDevice::Key::kKP_5: return ImGuiKey_Keypad5;
	case RE::BSKeyboardDevice::Key::kKP_6: return ImGuiKey_Keypad6;
	case RE::BSKeyboardDevice::Key::kKP_7: return ImGuiKey_Keypad7;
	case RE::BSKeyboardDevice::Key::kKP_8: return ImGuiKey_Keypad8;
	case RE::BSKeyboardDevice::Key::kKP_9: return ImGuiKey_Keypad9;	

	case RE::BSKeyboardDevice::Key::kKP_Divide:   return ImGuiKey_KeypadDivide;
	case RE::BSKeyboardDevice::Key::kKP_Multiply: return ImGuiKey_KeypadMultiply;
	case RE::BSKeyboardDevice::Key::kKP_Subtract: return ImGuiKey_KeypadSubtract;
	case RE::BSKeyboardDevice::Key::kKP_Plus:     return ImGuiKey_KeypadAdd;
	case RE::BSKeyboardDevice::Key::kKP_Enter:    return ImGuiKey_KeypadEnter;
	case RE::BSKeyboardDevice::Key::kKP_Decimal:  return ImGuiKey_KeypadDecimal;

	case 256: return ImGuiKey_MouseLeft;
	case 257: return ImGuiKey_MouseRight;
	case 258: return ImGuiKey_MouseMiddle;
	case 259: return ImGuiKey_MouseX1;
	case 260: return ImGuiKey_MouseX2;

	case 266: return ImGuiKey_GamepadDpadUp;
	case 267: return ImGuiKey_GamepadDpadDown;
	case 268: return ImGuiKey_GamepadDpadLeft;
	case 269: return ImGuiKey_GamepadDpadRight;
	case 270: return ImGuiKey_GamepadStart;
	case 271: return ImGuiKey_GamepadBack;
	case 272: return ImGuiKey_GamepadL3;
	case 273: return ImGuiKey_GamepadR3;
	case 274: return ImGuiKey_GamepadL1;
	case 275: return ImGuiKey_GamepadR1;
	case 276: return ImGuiKey_GamepadFaceDown;
	case 277: return ImGuiKey_GamepadFaceRight;
	case 278: return ImGuiKey_GamepadFaceLeft;
	case 279: return ImGuiKey_GamepadFaceUp;
	case 280: return ImGuiKey_GamepadL2;
	case 281: return ImGuiKey_GamepadR2;

	default: return ImGuiKey_None;
	}
}

int EDUI::ImGuiKeyToIDCode(ImGuiKey key)
{
	switch (key) {
    case ImGuiKey_A: return RE::BSKeyboardDevice::Key::kA;
    case ImGuiKey_B: return RE::BSKeyboardDevice::Key::kB;
    case ImGuiKey_C: return RE::BSKeyboardDevice::Key::kC;
    case ImGuiKey_D: return RE::BSKeyboardDevice::Key::kD;
    case ImGuiKey_E: return RE::BSKeyboardDevice::Key::kE;
    case ImGuiKey_F: return RE::BSKeyboardDevice::Key::kF;
    case ImGuiKey_G: return RE::BSKeyboardDevice::Key::kG;
    case ImGuiKey_H: return RE::BSKeyboardDevice::Key::kH;
    case ImGuiKey_I: return RE::BSKeyboardDevice::Key::kI;
    case ImGuiKey_J: return RE::BSKeyboardDevice::Key::kJ;
    case ImGuiKey_K: return RE::BSKeyboardDevice::Key::kK;
    case ImGuiKey_L: return RE::BSKeyboardDevice::Key::kL;
    case ImGuiKey_M: return RE::BSKeyboardDevice::Key::kM;
    case ImGuiKey_N: return RE::BSKeyboardDevice::Key::kN;
    case ImGuiKey_O: return RE::BSKeyboardDevice::Key::kO;
    case ImGuiKey_P: return RE::BSKeyboardDevice::Key::kP;
    case ImGuiKey_Q: return RE::BSKeyboardDevice::Key::kQ;
    case ImGuiKey_R: return RE::BSKeyboardDevice::Key::kR;
    case ImGuiKey_S: return RE::BSKeyboardDevice::Key::kS;
    case ImGuiKey_T: return RE::BSKeyboardDevice::Key::kT;
    case ImGuiKey_U: return RE::BSKeyboardDevice::Key::kU;
    case ImGuiKey_V: return RE::BSKeyboardDevice::Key::kV;
    case ImGuiKey_W: return RE::BSKeyboardDevice::Key::kW;
    case ImGuiKey_X: return RE::BSKeyboardDevice::Key::kX;
    case ImGuiKey_Y: return RE::BSKeyboardDevice::Key::kY;
    case ImGuiKey_Z: return RE::BSKeyboardDevice::Key::kZ;

    case ImGuiKey_0: return RE::BSKeyboardDevice::Key::kNum0;
    case ImGuiKey_1: return RE::BSKeyboardDevice::Key::kNum1;
    case ImGuiKey_2: return RE::BSKeyboardDevice::Key::kNum2;
    case ImGuiKey_3: return RE::BSKeyboardDevice::Key::kNum3;
    case ImGuiKey_4: return RE::BSKeyboardDevice::Key::kNum4;
    case ImGuiKey_5: return RE::BSKeyboardDevice::Key::kNum5;
    case ImGuiKey_6: return RE::BSKeyboardDevice::Key::kNum6;
    case ImGuiKey_7: return RE::BSKeyboardDevice::Key::kNum7;
    case ImGuiKey_8: return RE::BSKeyboardDevice::Key::kNum8;
    case ImGuiKey_9: return RE::BSKeyboardDevice::Key::kNum9;

    case ImGuiKey_F1:  return RE::BSKeyboardDevice::Key::kF1;
    case ImGuiKey_F2:  return RE::BSKeyboardDevice::Key::kF2;
    case ImGuiKey_F3:  return RE::BSKeyboardDevice::Key::kF3;
    case ImGuiKey_F4:  return RE::BSKeyboardDevice::Key::kF4;
    case ImGuiKey_F5:  return RE::BSKeyboardDevice::Key::kF5;
    case ImGuiKey_F6:  return RE::BSKeyboardDevice::Key::kF6;
    case ImGuiKey_F7:  return RE::BSKeyboardDevice::Key::kF7;
    case ImGuiKey_F8:  return RE::BSKeyboardDevice::Key::kF8;
    case ImGuiKey_F9:  return RE::BSKeyboardDevice::Key::kF9;
    case ImGuiKey_F10: return RE::BSKeyboardDevice::Key::kF10;
    case ImGuiKey_F11: return RE::BSKeyboardDevice::Key::kF11;
    case ImGuiKey_F12: return RE::BSKeyboardDevice::Key::kF12;

    case ImGuiKey_UpArrow:    return RE::BSKeyboardDevice::Key::kUp;
    case ImGuiKey_DownArrow:  return RE::BSKeyboardDevice::Key::kDown;
    case ImGuiKey_LeftArrow:  return RE::BSKeyboardDevice::Key::kLeft;
    case ImGuiKey_RightArrow: return RE::BSKeyboardDevice::Key::kRight;

    case ImGuiKey_Home:    return RE::BSKeyboardDevice::Key::kHome;
    case ImGuiKey_End:     return RE::BSKeyboardDevice::Key::kEnd;
    case ImGuiKey_Insert:  return RE::BSKeyboardDevice::Key::kInsert;
    case ImGuiKey_Delete:  return RE::BSKeyboardDevice::Key::kDelete;
    case ImGuiKey_PageUp:  return RE::BSKeyboardDevice::Key::kPageUp;
    case ImGuiKey_PageDown:return RE::BSKeyboardDevice::Key::kPageDown;

    case ImGuiKey_Escape:    return RE::BSKeyboardDevice::Key::kEscape;
    case ImGuiKey_Space:     return RE::BSKeyboardDevice::Key::kSpacebar;
    case ImGuiKey_Enter:     return RE::BSKeyboardDevice::Key::kEnter;
    case ImGuiKey_Tab:       return RE::BSKeyboardDevice::Key::kTab;
    case ImGuiKey_Backspace: return RE::BSKeyboardDevice::Key::kBackspace;

    case ImGuiKey_LeftShift:  return RE::BSKeyboardDevice::Key::kLeftShift;
    case ImGuiKey_RightShift: return RE::BSKeyboardDevice::Key::kRightShift;
    case ImGuiKey_LeftCtrl:   return RE::BSKeyboardDevice::Key::kLeftControl;
    case ImGuiKey_RightCtrl:  return RE::BSKeyboardDevice::Key::kRightControl;
    case ImGuiKey_LeftAlt:    return RE::BSKeyboardDevice::Key::kLeftAlt;
    case ImGuiKey_RightAlt:   return RE::BSKeyboardDevice::Key::kRightAlt;

	case ImGuiKey_ReservedForModShift: return RE::BSKeyboardDevice::Key::kLeftShift;
	case ImGuiKey_ReservedForModCtrl: return RE::BSKeyboardDevice::Key::kLeftControl;
	case ImGuiKey_ReservedForModAlt: return RE::BSKeyboardDevice::Key::kLeftAlt;

    case ImGuiKey_Equal:        return RE::BSKeyboardDevice::Key::kEquals;
    case ImGuiKey_Minus:        return RE::BSKeyboardDevice::Key::kMinus;
    case ImGuiKey_LeftBracket:  return RE::BSKeyboardDevice::Key::kBracketLeft;
    case ImGuiKey_RightBracket: return RE::BSKeyboardDevice::Key::kBracketRight;
    case ImGuiKey_Backslash:    return RE::BSKeyboardDevice::Key::kBackslash;
    case ImGuiKey_Semicolon:    return RE::BSKeyboardDevice::Key::kSemicolon;
    case ImGuiKey_Apostrophe:   return RE::BSKeyboardDevice::Key::kApostrophe;
    case ImGuiKey_Comma:        return RE::BSKeyboardDevice::Key::kComma;
    case ImGuiKey_Period:       return RE::BSKeyboardDevice::Key::kPeriod;
    case ImGuiKey_Slash:        return RE::BSKeyboardDevice::Key::kSlash;
    case ImGuiKey_GraveAccent:  return RE::BSKeyboardDevice::Key::kTilde;

    case ImGuiKey_CapsLock:    return RE::BSKeyboardDevice::Key::kCapsLock;
    case ImGuiKey_NumLock:     return RE::BSKeyboardDevice::Key::kNumLock;
    case ImGuiKey_ScrollLock:  return RE::BSKeyboardDevice::Key::kScrollLock;
    case ImGuiKey_Pause:       return RE::BSKeyboardDevice::Key::kPause;

    case ImGuiKey_Keypad0: return RE::BSKeyboardDevice::Key::kKP_0;
    case ImGuiKey_Keypad1: return RE::BSKeyboardDevice::Key::kKP_1;
    case ImGuiKey_Keypad2: return RE::BSKeyboardDevice::Key::kKP_2;
    case ImGuiKey_Keypad3: return RE::BSKeyboardDevice::Key::kKP_3;
    case ImGuiKey_Keypad4: return RE::BSKeyboardDevice::Key::kKP_4;
    case ImGuiKey_Keypad5: return RE::BSKeyboardDevice::Key::kKP_5;
    case ImGuiKey_Keypad6: return RE::BSKeyboardDevice::Key::kKP_6;
    case ImGuiKey_Keypad7: return RE::BSKeyboardDevice::Key::kKP_7;
    case ImGuiKey_Keypad8: return RE::BSKeyboardDevice::Key::kKP_8;
    case ImGuiKey_Keypad9: return RE::BSKeyboardDevice::Key::kKP_9;

    case ImGuiKey_KeypadDivide:		return RE::BSKeyboardDevice::Key::kKP_Divide;
    case ImGuiKey_KeypadMultiply:	return RE::BSKeyboardDevice::Key::kKP_Multiply;
    case ImGuiKey_KeypadSubtract:	return RE::BSKeyboardDevice::Key::kKP_Subtract;
    case ImGuiKey_KeypadAdd:		return RE::BSKeyboardDevice::Key::kKP_Plus;
	case ImGuiKey_KeypadEnter:		return RE::BSKeyboardDevice::Key::kKP_Enter;
	case ImGuiKey_KeypadDecimal:	return RE::BSKeyboardDevice::Key::kKP_Decimal;

	case ImGuiKey_MouseLeft:		return 256;
	case ImGuiKey_MouseRight:		return 257;
	case ImGuiKey_MouseMiddle:		return 258;
	case ImGuiKey_MouseX1:			return 259;
	case ImGuiKey_MouseX2:			return 260;

	case ImGuiKey_GamepadDpadUp:	return 266;
	case ImGuiKey_GamepadDpadDown:  return 267;
	case ImGuiKey_GamepadDpadLeft:  return 268;
	case ImGuiKey_GamepadDpadRight: return 269;
	case ImGuiKey_GamepadStart:     return 270;
	case ImGuiKey_GamepadBack:      return 271;
	case ImGuiKey_GamepadL3:		return 272;
	case ImGuiKey_GamepadR3:		return 273;
	case ImGuiKey_GamepadL1:		return 274;
	case ImGuiKey_GamepadR1:		return 275;
	case ImGuiKey_GamepadFaceDown:  return 276;
	case ImGuiKey_GamepadFaceRight: return 277;
	case ImGuiKey_GamepadFaceLeft:  return 278;
	case ImGuiKey_GamepadFaceUp:    return 279;
	case ImGuiKey_GamepadL2:		return 280;
	case ImGuiKey_GamepadR2:		return 281;

	default: return -1;
	}
}