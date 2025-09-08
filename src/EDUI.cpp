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
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_HeavyArmor", &Settings->ED_Degrade_HeavyArmor, 0.0, 10.0, "%.01f");
			ImGui::TableSetColumnIndex(2); ImGui::SliderInt("##Break_HeavyArmor", &Settings->ED_Break_HeavyArmor, 0, 100, "%d%%");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Clothing");
			ImGui::TableSetColumnIndex(1); ImGui::SliderFloat("##Degrade_Clothing", &Settings->ED_Degrade_Clothing, 0.0, 10.0, "%.01f");
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
					durabilityMenu->ShowHideMenu();
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

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Checkbox("Show Unarmed", &Settings->ED_Widget_ShowUnarmed);
			ImGui::TableSetColumnIndex(1); ImGui::Text("");

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
		if (ImGui::Button(waitKey ? "Press a key..." : ImGui::GetKeyName((ImGuiKey)HelperFunctions::IDCodeToImGuiKey(hotkey)))) {
			waitKey = true;
			hotkey = -1;
		}

		// If waiting, check for input
		if (waitKey) {
			for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
				if (ImGui::IsKeyPressed((ImGuiKey)key)) {
					hotkey = HelperFunctions::ImGuiKeyToIDCode((ImGuiKey)key);
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

		ImGui::Text("  Display Duration (0 is persistent)");
		ImGui::SliderInt("##Hotkey_Duration", &Settings->ED_Widget_ToggleDuration, 0, 30);
	}
}

void EDUI::SliderEntryMaterial(float& value, const char* id) {
	int tempValue = value * 100;
	if (ImGui::SliderInt(id, &tempValue, 0, 200, "%d%%"))
		value = tempValue / 100.0f;
}