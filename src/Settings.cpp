#include "Settings.h"
#include "Utility.h"

// Settings
Settings* Settings::GetSingleton() {
	static Settings singleton;
	return std::addressof(singleton);
}

void Settings::LoadINI() {
	CSimpleIniA iniSettings;
	iniSettings.SetUnicode();
	iniSettings.LoadFile(setting_path);

	// System Settings
	get_value(iniSettings, ED_DegradationDisabled, "Degradation", "DisableDegradation");
	get_value(iniSettings, ED_BreakDisabled, "Degradation", "DisableBreak");
	get_value(iniSettings, ED_OnlyPlayer, "Degradation", "OnlyPlayer");
	get_value(iniSettings, ED_NoBreakNoEnchant, "Degradation", "NoBreakMagicDisallowEnchanting");
	get_value(iniSettings, ED_IncreasedDurability, "Degradation", "IncreaseDurability");
	get_value(iniSettings, ED_Material_Multiplier, "Degradation", "MaterialMultiplier");
	get_value(iniSettings, ED_BreakThreshold, "Degradation", "BreakThreshold");

	// Degredaton Rates
	get_value(iniSettings, ED_Degrade_Sword, "DegradationRate", "WeaponSword");
	get_value(iniSettings, ED_Degrade_Dagger, "DegradationRate", "WeaponDagger");
	get_value(iniSettings, ED_Degrade_WarAxe, "DegradationRate", "WeaponWarAxe");
	get_value(iniSettings, ED_Degrade_Mace, "DegradationRate", "WeaponMace");
	get_value(iniSettings, ED_Degrade_GreatSword, "DegradationRate", "WeaponGreatSword");
	get_value(iniSettings, ED_Degrade_Warhammer, "DegradationRate", "WeaponHammer");
	get_value(iniSettings, ED_Degrade_BattleAxe, "DegradationRate", "WeaponBattleAxe");
	get_value(iniSettings, ED_Degrade_Bow, "DegradationRate", "WeaponBow");
	get_value(iniSettings, ED_Degrade_CrossBow, "DegradationRate", "WeaponCrossbow");
	get_value(iniSettings, ED_Degrade_LightArmor, "DegradationRate", "LightArmor");
	get_value(iniSettings, ED_Degrade_HeavyArmor, "DegradationRate", "HeavyArmor");
	get_value(iniSettings, ED_Degrade_Clothing, "DegradationRate", "Clothing");
	get_value(iniSettings, ED_Degrade_Armor, "DegradationRate", "DefaultArmor");
	get_value(iniSettings, ED_Degrade_PowerAttack, "DegradationRate", "PowerAttackMultiplier");
	get_value(iniSettings, ED_Degrade_FollowerMulti, "DegradationRate", "FollowerMultiplier");
	get_value(iniSettings, ED_Degrade_NPCMulti, "DegradationRate", "NPCMultiplier");

	// Break Rates
	get_value(iniSettings, ED_Break_Sword, "BreakChance", "WeaponSword");
	get_value(iniSettings, ED_Break_Dagger, "BreakChance", "WeaponDagger");
	get_value(iniSettings, ED_Break_WarAxe, "BreakChance", "WeaponWarAxe");
	get_value(iniSettings, ED_Break_Mace, "BreakChance", "WeaponMace");
	get_value(iniSettings, ED_Break_GreatSword, "BreakChance", "WeaponGreatSword");
	get_value(iniSettings, ED_Break_Warhammer, "BreakChance", "WeaponHammer");
	get_value(iniSettings, ED_Break_BattleAxe, "BreakChance", "WeaponBattleAxe");
	get_value(iniSettings, ED_Break_Bow, "BreakChance", "WeaponBow");
	get_value(iniSettings, ED_Break_CrossBow, "BreakChance", "WeaponCrossbow");
	get_value(iniSettings, ED_Break_LightArmor, "BreakChance", "LightArmor");
	get_value(iniSettings, ED_Break_HeavyArmor, "BreakChance", "HeavyArmor");
	get_value(iniSettings, ED_Break_Clothing, "BreakChance", "Clothing");
	get_value(iniSettings, ED_Break_Armor, "BreakChance", "DefaultArmor");
	get_value(iniSettings, ED_Break_PowerAttack, "BreakChance", "PowerAttackMultiplier");
	get_value(iniSettings, ED_Break_FollowerMulti, "BreakChance", "FollowerMultiplier");
	get_value(iniSettings, ED_Break_NPCMulti, "BreakChance", "NPCMultiplier");

	// Material Rates
	get_value(iniSettings, ED_Daedric, "MaterialMultiplier", "Daedric");
	get_value(iniSettings, ED_Dragon, "MaterialMultiplier", "Dragon");
	get_value(iniSettings, ED_Ebony, "MaterialMultiplier", "Ebony");
	get_value(iniSettings, ED_Stalhrim, "MaterialMultiplier", "Stalhrim");
	get_value(iniSettings, ED_Orcish, "MaterialMultiplier", "Orcish");
	get_value(iniSettings, ED_Glass, "MaterialMultiplier", "Glass");
	get_value(iniSettings, ED_Dwarven, "MaterialMultiplier", "Dwarven");
	get_value(iniSettings, ED_Chitin, "MaterialMultiplier", "Chitin");
	get_value(iniSettings, ED_Falmer, "MaterialMultiplier", "Falmer");
	get_value(iniSettings, ED_Bonemold, "MaterialMultiplier", "Bonemold");
	get_value(iniSettings, ED_Elven, "MaterialMultiplier", "Elven");
	get_value(iniSettings, ED_Silver, "MaterialMultiplier", "Silver");
	get_value(iniSettings, ED_Steel, "MaterialMultiplier", "Steel");
	get_value(iniSettings, ED_Iron, "MaterialMultiplier", "Iron");
	get_value(iniSettings, ED_Leather, "MaterialMultiplier", "Leather");
	get_value(iniSettings, ED_Fur, "MaterialMultiplier", "Fur");

	// Dynamic Temper
	get_value(iniSettings, ED_Temper_Enabled, "DynamicTemper", "EnableDynamicTemper");
	get_value(iniSettings, ED_Temper_Chance, "DynamicTemper", "TemperChance");
	get_value(iniSettings, ED_Temper_VendorChance, "DynamicTemper", "VendorTemperChance");
	get_value(iniSettings, ED_Temper_BossChance, "DynamicTemper", "BossTemperChance");

	// Dynamic Enchanting
	get_value(iniSettings, ED_Enchant_Enabled, "DynamicEnchant", "EnableDynamicEnchant");
	get_value(iniSettings, ED_Enchant_Chance, "DynamicEnchant", "EnchantChance");
	get_value(iniSettings, ED_Enchant_VendorChance, "DynamicEnchant", "VendorEnchantChance");
	get_value(iniSettings, ED_Enchant_BossChance, "DynamicEnchant", "BossEnchantChance");

	// Widget Position and Color
	get_value(iniSettings, ED_Widget_Display, "Widget", "DisplayType");
	get_value(iniSettings, ED_Widget_PosX, "Widget", "PositionX");
	get_value(iniSettings, ED_Widget_PosY, "Widget", "PositionY");
	get_value(iniSettings, ED_Widget_Scale, "Widget", "Scale");
	get_value(iniSettings, ED_Color_Unbreakable, "Widget", "UnbreakableColor");
	get_value(iniSettings, ED_Color_Broken, "Widget", "BreakableColor");

	// Widget Hotkeys
	get_value(iniSettings, ED_Widget_ToggleKeyCode, "Widget", "ToggleKeyCode");
	get_value(iniSettings, ED_Widget_ToggleDuration, "Widget", "ToggleDuration");

	// Widget Layout Options
	get_value(iniSettings, ED_Widget_Reverse, "Widget", "Reverse");
	get_value(iniSettings, ED_Widget_ShowPoisonName, "Widget", "ShowPoisonName");
	get_value(iniSettings, ED_Widget_ShowShout, "Widget", "ShowShout");
	get_value(iniSettings, ED_Widget_ShowWeaponName, "Widget", "ShowWeaponName");
	get_value(iniSettings, ED_Widget_ShowArmorName, "Widget", "ShowArmorName");
	get_value(iniSettings, ED_Widget_ShowHealth, "Widget", "ShowHealth");

	iniSettings.SaveFile(setting_path);

	// Set the material map
	materialData = {
		{"Daedric",  ED_Daedric},
		{"Dragon",   ED_Dragon},
		{"Ebony",    ED_Ebony},
		{"Stalhrim", ED_Stalhrim},
		{"Orcish",   ED_Orcish},
		{"Glass",    ED_Glass},
		{"Dwarven",  ED_Dwarven},
		{"Chitin",   ED_Chitin},
		{"Falmer",   ED_Falmer},
		{"Bonemold", ED_Bonemold},
		{"Elven",    ED_Elven},
		{"Silver",   ED_Silver},
		{"Steel",    ED_Steel},
		{"Iron",     ED_Iron},
		{"Leather",  ED_Leather},
		{"Fur",      ED_Fur}
	};

	// Process the rest of the lists
	ProcessNoBreakForms();
	ProcessEnchantingForms();
	ProcessMaterialForms();
	SetVendorList();
}

void Settings::SaveINI() {
	CSimpleIniA iniSettings;
	iniSettings.SetUnicode();
	iniSettings.LoadFile(setting_path);

	// System Settings
	set_value(iniSettings, ED_DegradationDisabled, "Degradation", "DisableDegradation");
	set_value(iniSettings, ED_BreakDisabled, "Degradation", "DisableBreak");
	set_value(iniSettings, ED_OnlyPlayer, "Degradation", "OnlyPlayer");
	set_value(iniSettings, ED_NoBreakNoEnchant, "Degradation", "NoBreakMagicDisallowEnchanting");
	set_value(iniSettings, ED_IncreasedDurability, "Degradation", "IncreaseDurability");
	set_value(iniSettings, ED_Material_Multiplier, "Degradation", "MaterialMultiplier");
	set_value(iniSettings, ED_BreakThreshold, "Degradation", "BreakThreshold");

	// Degredaton Rates
	set_value(iniSettings, ED_Degrade_Sword, "DegradationRate", "WeaponSword");
	set_value(iniSettings, ED_Degrade_Dagger, "DegradationRate", "WeaponDagger");
	set_value(iniSettings, ED_Degrade_WarAxe, "DegradationRate", "WeaponWarAxe");
	set_value(iniSettings, ED_Degrade_Mace, "DegradationRate", "WeaponMace");
	set_value(iniSettings, ED_Degrade_GreatSword, "DegradationRate", "WeaponGreatSword");
	set_value(iniSettings, ED_Degrade_Warhammer, "DegradationRate", "WeaponHammer");
	set_value(iniSettings, ED_Degrade_BattleAxe, "DegradationRate", "WeaponBattleAxe");
	set_value(iniSettings, ED_Degrade_Bow, "DegradationRate", "WeaponBow");
	set_value(iniSettings, ED_Degrade_CrossBow, "DegradationRate", "WeaponCrossbow");
	set_value(iniSettings, ED_Degrade_LightArmor, "DegradationRate", "LightArmor");
	set_value(iniSettings, ED_Degrade_HeavyArmor, "DegradationRate", "HeavyArmor");
	set_value(iniSettings, ED_Degrade_Clothing, "DegradationRate", "Clothing");
	set_value(iniSettings, ED_Degrade_Armor, "DegradationRate", "DefaultArmor");
	set_value(iniSettings, ED_Degrade_PowerAttack, "DegradationRate", "PowerAttackMultiplier");
	set_value(iniSettings, ED_Degrade_FollowerMulti, "DegradationRate", "FollowerMultiplier");
	set_value(iniSettings, ED_Degrade_NPCMulti, "DegradationRate", "NPCMultiplier");

	// Break Rates
	set_value(iniSettings, ED_Break_Sword, "BreakChance", "WeaponSword");
	set_value(iniSettings, ED_Break_Dagger, "BreakChance", "WeaponDagger");
	set_value(iniSettings, ED_Break_WarAxe, "BreakChance", "WeaponWarAxe");
	set_value(iniSettings, ED_Break_Mace, "BreakChance", "WeaponMace");
	set_value(iniSettings, ED_Break_GreatSword, "BreakChance", "WeaponGreatSword");
	set_value(iniSettings, ED_Break_Warhammer, "BreakChance", "WeaponHammer");
	set_value(iniSettings, ED_Break_BattleAxe, "BreakChance", "WeaponBattleAxe");
	set_value(iniSettings, ED_Break_Bow, "BreakChance", "WeaponBow");
	set_value(iniSettings, ED_Break_CrossBow, "BreakChance", "WeaponCrossbow");
	set_value(iniSettings, ED_Break_LightArmor, "BreakChance", "LightArmor");
	set_value(iniSettings, ED_Break_HeavyArmor, "BreakChance", "HeavyArmor");
	set_value(iniSettings, ED_Break_Clothing, "BreakChance", "Clothing");
	set_value(iniSettings, ED_Break_Armor, "BreakChance", "DefaultArmor");
	set_value(iniSettings, ED_Break_PowerAttack, "BreakChance", "PowerAttackMultiplier");
	set_value(iniSettings, ED_Break_FollowerMulti, "BreakChance", "FollowerMultiplier");
	set_value(iniSettings, ED_Break_NPCMulti, "BreakChance", "NPCMultiplier");

	// Material Rates
	set_value(iniSettings, ED_Daedric, "MaterialMultiplier", "Daedric");
	set_value(iniSettings, ED_Dragon, "MaterialMultiplier", "Dragon");
	set_value(iniSettings, ED_Ebony, "MaterialMultiplier", "Ebony");
	set_value(iniSettings, ED_Stalhrim, "MaterialMultiplier", "Stalhrim");
	set_value(iniSettings, ED_Orcish, "MaterialMultiplier", "Orcish");
	set_value(iniSettings, ED_Glass, "MaterialMultiplier", "Glass");
	set_value(iniSettings, ED_Dwarven, "MaterialMultiplier", "Dwarven");
	set_value(iniSettings, ED_Chitin, "MaterialMultiplier", "Chitin");
	set_value(iniSettings, ED_Falmer, "MaterialMultiplier", "Falmer");
	set_value(iniSettings, ED_Bonemold, "MaterialMultiplier", "Bonemold");
	set_value(iniSettings, ED_Elven, "MaterialMultiplier", "Elven");
	set_value(iniSettings, ED_Silver, "MaterialMultiplier", "Silver");
	set_value(iniSettings, ED_Steel, "MaterialMultiplier", "Steel");
	set_value(iniSettings, ED_Iron, "MaterialMultiplier", "Iron");
	set_value(iniSettings, ED_Leather, "MaterialMultiplier", "Leather");
	set_value(iniSettings, ED_Fur, "MaterialMultiplier", "Fur");

	// Dynamic Temper
	set_value(iniSettings, ED_Temper_Enabled, "DynamicTemper", "EnableDynamicTemper");
	set_value(iniSettings, ED_Temper_Chance, "DynamicTemper", "TemperChance");
	set_value(iniSettings, ED_Temper_VendorChance, "DynamicTemper", "VendorTemperChance");
	set_value(iniSettings, ED_Temper_BossChance, "DynamicTemper", "BossTemperChance");

	// Dynamic Enchanting
	set_value(iniSettings, ED_Enchant_Enabled, "DynamicEnchant", "EnableDynamicEnchant");
	set_value(iniSettings, ED_Enchant_Chance, "DynamicEnchant", "EnchantChance");
	set_value(iniSettings, ED_Enchant_VendorChance, "DynamicEnchant", "VendorEnchantChance");
	set_value(iniSettings, ED_Enchant_BossChance, "DynamicEnchant", "BossEnchantChance");

	// Widget Position and Color
	set_value(iniSettings, ED_Widget_Display, "Widget", "DisplayType");
	set_value(iniSettings, ED_Widget_PosX, "Widget", "PositionX");
	set_value(iniSettings, ED_Widget_PosY, "Widget", "PositionY");
	set_value(iniSettings, ED_Widget_Scale, "Widget", "Scale");
	set_value(iniSettings, ED_Color_Unbreakable, "Widget", "UnbreakableColor");
	set_value(iniSettings, ED_Color_Broken, "Widget", "BreakableColor");

	// Widget Hotkeys
	set_value(iniSettings, ED_Widget_ToggleKeyCode, "Widget", "ToggleKeyCode");
	set_value(iniSettings, ED_Widget_ToggleDuration, "Widget", "ToggleDuration");

	// Widget Layout Options
	set_value(iniSettings, ED_Widget_Reverse, "Widget", "Reverse");
	set_value(iniSettings, ED_Widget_ShowPoisonName, "Widget", "ShowPoisonName");
	set_value(iniSettings, ED_Widget_ShowShout, "Widget", "ShowShout");
	set_value(iniSettings, ED_Widget_ShowWeaponName, "Widget", "ShowWeaponName");
	set_value(iniSettings, ED_Widget_ShowArmorName, "Widget", "ShowArmorName");
	set_value(iniSettings, ED_Widget_ShowHealth, "Widget", "ShowHealth");

	iniSettings.SaveFile(setting_path);
}

void Settings::ProcessNoBreakForms() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetMultiKey(true);
	ini.LoadFile(list_path);

	// Get all keys, and return if there are none
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllValues("BreakList", "BreakEntry", keys);
	if (keys.empty()) return;

	// Split the string and map accordingly
	for (auto& kv : keys) {
		// 0 - ESP Name, 1 - FormID
		std::vector<std::string> parts = SplitString(kv.pItem, '|');
		auto index = RE::TESDataHandler::GetSingleton()->GetModIndex(parts[0]);
		if (index.value() != 0xFF) noBreakForms.insert((index.value() << 24) + std::stoull(parts[1], nullptr, 0));
	}

	logger::info("Loaded {} No Break Forms", noBreakForms.size());
}

void Settings::ProcessEnchantingForms() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetMultiKey(true);
	ini.LoadFile(list_path);

	// Get all keys, and return if there are none
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllValues("EnchantmentList", "EnchantEntry", keys);
	if (keys.empty()) return;

	// Send the enchantments to their respective lists
	for (auto& kv : keys) {
		// 0 - ESP Name, 1 - FormID, 2 - Body Part, 3 - Enchant Suffix
		std::vector<std::string> parts = SplitString(kv.pItem, '|');
		if (auto* newItem = RE::TESDataHandler::GetSingleton()->LookupForm(RE::FormID(std::stoull(parts[1], nullptr, 0)), parts[0])) {
			// Set our enchantment values
			Enchantments newEnch;
			newEnch.enchantment = newItem->As<RE::EnchantmentItem>();
			newEnch.name =  parts[3];

			// Add the enchantment to the correct lists
			if (parts[2].rfind("weapon") != std::string::npos)
				enchantWeapon.push_back(newEnch);
			if (parts[2].rfind("head") != std::string::npos)
				enchantHead.push_back(newEnch);
			if (parts[2].rfind("body") != std::string::npos)
				enchantBody.push_back(newEnch);
			if (parts[2].rfind("hand") != std::string::npos)
				enchantHand.push_back(newEnch);
			if (parts[2].rfind("foot") != std::string::npos)
				enchantFoot.push_back(newEnch);
			if (parts[2].rfind("shield") != std::string::npos)
				enchantShield.push_back(newEnch);
		}
	}

	logger::info("Loaded {} Weapon Enchantments", enchantWeapon.size());
	logger::info("Loaded {} Head Enchantments", enchantHead.size());
	logger::info("Loaded {} Body Enchantments", enchantBody.size());
	logger::info("Loaded {} Hand Enchantments", enchantHand.size());
	logger::info("Loaded {} Foot Enchantments", enchantFoot.size());
	logger::info("Loaded {} Shield Enchantments", enchantShield.size());
}

void Settings::ProcessMaterialForms() {
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.SetMultiKey(true);
	ini.LoadFile(list_path);

	// Get all keys, and return if there are none
	CSimpleIniA::TNamesDepend keys;
	ini.GetAllValues("MaterialList", "MatEntry", keys);
	if (keys.empty()) return;

	for (auto& kv : keys) {
		// 0 - ESP Name, 1 - FormID, 2 - Material
		std::vector<std::string> parts = SplitString(kv.pItem, '|');

		if (auto* newItem = RE::TESDataHandler::GetSingleton()->LookupForm(RE::FormID(std::stoull(parts[1], nullptr, 0)), parts[0])) {
			std::string editorID = newItem->GetFormEditorID();

			// No Editor ID
			if (editorID == "_None") continue;

			// If Editor ID does not exist, add it to the map
			auto it = materialMap.find(editorID);
			if (it == materialMap.end()) {
				materialMap.emplace(editorID, parts[2]);
			}
		}
	}
}

// Material Setting
double Settings::MaterialRate(std::span<RE::BGSKeyword*> keywords) {
	int iKeywordCount = 0;
	double dRates = 0;

	// If Material Multiplier is on
	if (ED_Material_Multiplier) {
		for (RE::BGSKeyword* keyword : keywords) {
			std::string tmp = keyword->GetFormEditorID();

			if (materialMap.count(tmp) >= 1) {
				dRates += materialData.at(materialMap.at(tmp));
				iKeywordCount++;
			}
		}

		double finalRate = dRates / iKeywordCount;
		return (finalRate > 0) ? finalRate : 1.0;

	} else
		return 1.0;
}

#pragma region "Public Functions"

double Settings::GetDegradationRate(RE::TESForm* form) {
	auto utility = Utility::GetSingleton();

	if (form->formType == RE::FormType::Weapon) {
		RE::TESObjectWEAP *weap = form->As<RE::TESObjectWEAP>();

		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Degradation Rate
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword: return ED_Degrade_Sword * matMulti;
			case RE::WEAPON_TYPE::kOneHandDagger: return ED_Degrade_Dagger * matMulti;
			case RE::WEAPON_TYPE::kOneHandAxe: return ED_Degrade_WarAxe * matMulti;
			case RE::WEAPON_TYPE::kOneHandMace: return ED_Degrade_Mace * matMulti;
			case RE::WEAPON_TYPE::kTwoHandSword: return ED_Degrade_GreatSword * matMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer))
					return ED_Degrade_Warhammer * matMulti;
				else
					return ED_Degrade_BattleAxe * matMulti;
			case RE::WEAPON_TYPE::kBow: return ED_Degrade_Bow * matMulti;
			case RE::WEAPON_TYPE::kCrossbow: return ED_Degrade_CrossBow * matMulti;
			default: return ED_Degrade_Sword * matMulti;
		}
	} else if (form->formType == RE::FormType::Armor) {
		RE::TESObjectARMO *armor = form->As<RE::TESObjectARMO>();

		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Degradation Rate
		if (armor->IsLightArmor()) return ED_Degrade_LightArmor * matMulti;
		else if (armor->IsHeavyArmor()) return ED_Degrade_HeavyArmor * matMulti;
		else if (armor->HasKeyword(utility->keywordClothing)) return ED_Degrade_Clothing * matMulti;
	}

	return ED_Degrade_Armor;
}

double Settings::GetBreakChance(RE::TESForm* form) {
	auto utility = Utility::GetSingleton();

	if (form->formType == RE::FormType::Weapon) {
		RE::TESObjectWEAP *weap = form->As<RE::TESObjectWEAP>();

		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Break Chance
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword: return ED_Break_Sword * matMulti;
			case RE::WEAPON_TYPE::kOneHandDagger: return ED_Break_Dagger * matMulti;
			case RE::WEAPON_TYPE::kOneHandAxe: return ED_Break_WarAxe * matMulti;
			case RE::WEAPON_TYPE::kOneHandMace: return ED_Break_Mace * matMulti;
			case RE::WEAPON_TYPE::kTwoHandSword: return ED_Break_GreatSword * matMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer))
					return ED_Break_Warhammer * matMulti;
				else
					return ED_Break_BattleAxe * matMulti;
			case RE::WEAPON_TYPE::kBow: return ED_Break_Bow * matMulti;
			case RE::WEAPON_TYPE::kCrossbow: return ED_Break_CrossBow * matMulti;
			default: return ED_Break_Sword * matMulti;
		}
	} else if (form->formType == RE::FormType::Armor) {
		RE::TESObjectARMO *armor = form->As<RE::TESObjectARMO>();

		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Break Chance
		if (armor->IsLightArmor()) return ED_Break_LightArmor * matMulti;
		else if (armor->IsHeavyArmor()) return ED_Break_HeavyArmor * matMulti;
		else if (armor->HasKeyword(utility->keywordClothing)) return ED_Break_Clothing * matMulti;
	}

	return ED_Break_Armor;
}

// Enchant List Functions
std::vector<Enchantments>* Settings::GetEnchantmentList(std::string part) {
	// Select correct vector based on part
	if (stricmp(part.c_str(),"weapon"))
		return &enchantWeapon;
	else if (stricmp(part.c_str(),"head"))
		return &enchantHead;
	else if (stricmp(part.c_str(),"body"))
		return &enchantBody;
	else if (stricmp(part.c_str(),"hand"))
		return &enchantHand;
	else if (stricmp(part.c_str(),"foot"))
		return &enchantFoot;
	else if (stricmp(part.c_str(),"shield"))
		return &enchantShield;

	return nullptr;
}

RE::EnchantmentItem* Settings::GetEnchantmentForm(std::string part, int index) {
	Enchantments foundEnchant;

	// Add the enchantment to the correct lists
	if (stricmp(part.c_str(),"weapon"))
		foundEnchant = enchantWeapon.at(index);
	else if (stricmp(part.c_str(),"head"))
		foundEnchant = enchantHead.at(index);
	else if (stricmp(part.c_str(),"body"))
		foundEnchant = enchantBody.at(index);
	else if (stricmp(part.c_str(),"hand"))
		foundEnchant = enchantHand.at(index);
	else if (stricmp(part.c_str(),"foot"))
		foundEnchant = enchantFoot.at(index);
	else if (stricmp(part.c_str(),"shield"))
		foundEnchant = enchantShield.at(index);

	return foundEnchant.enchantment;
}

int Settings::GetEnchantmentSize(std::string part) {
	if (stricmp(part.c_str(),"weapon"))
		return enchantWeapon.size();
	else if (stricmp(part.c_str(),"head"))
		return enchantHead.size();
	else if (stricmp(part.c_str(),"body"))
		return enchantBody.size();
	else if (stricmp(part.c_str(),"hand"))
		return enchantHand.size();
	else if (stricmp(part.c_str(),"foot"))
		return enchantFoot.size();
	else if (stricmp(part.c_str(),"shield"))
		return enchantShield.size();

	return 0;
}

std::string Settings::GetEnchantmentName(std::string part, int index) {
	Enchantments foundEnchant;

	// Add the enchantment to the correct lists
	if (stricmp(part.c_str(),"weapon"))
		foundEnchant = enchantWeapon.at(index);
	else if (stricmp(part.c_str(),"head"))
		foundEnchant = enchantHead.at(index);
	else if (stricmp(part.c_str(),"body"))
		foundEnchant = enchantBody.at(index);
	else if (stricmp(part.c_str(),"hand"))
		foundEnchant = enchantHand.at(index);
	else if (stricmp(part.c_str(),"foot"))
		foundEnchant = enchantFoot.at(index);
	else if (stricmp(part.c_str(),"shield"))
		foundEnchant = enchantShield.at(index);

	return foundEnchant.name;
}

// Break Form Functions
bool Settings::HasNoBreakForms(int formid) {
	if (noBreakForms.empty()) return false;

	auto it = noBreakForms.find(formid);
	if (it == noBreakForms.end()) return false;

	return true;
}

// Vendor Container Functions
bool Settings::IsVendorContainer(RE::TESObjectREFR* form) {
	if (std::find(vendorContainers.begin(), vendorContainers.end(), form) != vendorContainers.end())
		return true;
	else
		return false;
}

void Settings::SetVendorList() {
	// Get all factions
	RE::BSTArray<RE::TESForm *> factionList = RE::TESDataHandler::GetSingleton()->GetFormArray(RE::FormType::Faction);

	// Loop through all found factions
	for (RE::TESForm * faction : factionList) {
		RE::TESFaction* foundFaction = faction->As<RE::TESFaction>();

		if (foundFaction->IsVendor() && foundFaction->vendorData.merchantContainer)
			if (!(std::find(vendorContainers.begin(), vendorContainers.end(), foundFaction->vendorData.merchantContainer) != vendorContainers.end())) {
				vendorContainers.push_back(foundFaction->vendorData.merchantContainer);
			}
	}

	logger::debug("Vendor Containers Found: {}", vendorContainers.size());
}

// String Functions
std::vector<std::string> Settings::SplitString(const std::string& s, char delimiter = '|')
{
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter))
        tokens.push_back(item);
    return tokens;
}

// INI Functions
void Settings::get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key) {
	if (a_ini.KeyExists(a_section, a_key))
		a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
	else
		a_ini.SetBoolValue(a_section, a_key, a_value);
};

void Settings::get_value(CSimpleIniA& a_ini, std::string& a_value, const char* a_section, const char* a_key) {
	char const *pchar = a_value.c_str();
	if (a_ini.KeyExists(a_section, a_key))
		a_value = a_ini.GetValue(a_section, a_key, pchar);
	else
		a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::get_value(CSimpleIniA& a_ini, int& a_value, const char* a_section, const char* a_key) {
	std::string s = std::to_string(a_value);
	char const *pchar = s.c_str();
	if (a_ini.KeyExists(a_section, a_key))
		a_value = std::stoi(a_ini.GetValue(a_section, a_key, pchar));
	else
		a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::get_value(CSimpleIniA& a_ini, float& a_value, const char* a_section, const char* a_key) {
	std::string s = std::to_string(a_value);
	char const *pchar = s.c_str();
	if (a_ini.KeyExists(a_section, a_key))
		a_value = std::stof(a_ini.GetValue(a_section, a_key, pchar));
	else
		a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::get_value(CSimpleIniA& a_ini, std::uint32_t& a_value, const char* a_section, const char* a_key) {
	std::string s = std::format("0x{:X}", a_value);
	char const *pchar = s.c_str();
	if (a_ini.KeyExists(a_section, a_key))
		a_value = std::stoul(a_ini.GetValue(a_section, a_key, pchar),nullptr,0);
	else
		a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::set_value(CSimpleIniA& a_ini, bool a_value, const char* a_section, const char* a_key) {
	a_ini.SetBoolValue(a_section, a_key, a_value);
};

void Settings::set_value(CSimpleIniA& a_ini, std::string a_value, const char* a_section, const char* a_key) {
	char const *pchar = a_value.c_str();
	a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::set_value(CSimpleIniA& a_ini, int a_value, const char* a_section, const char* a_key) {
	std::string s = std::to_string(a_value);
	char const *pchar = s.c_str();
	a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::set_value(CSimpleIniA& a_ini, float a_value, const char* a_section, const char* a_key) {
	std::string s = std::to_string(a_value);
	char const *pchar = s.c_str();
	a_ini.SetValue(a_section, a_key, pchar);
};

void Settings::set_value(CSimpleIniA& a_ini, std::uint32_t a_value, const char* a_section, const char* a_key) {
	std::string s = std::format("0x{:X}", a_value);
	char const *pchar = s.c_str();
	a_ini.SetValue(a_section, a_key, pchar);
};