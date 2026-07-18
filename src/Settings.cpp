#include "Settings.h"
#include "Utility.h"

// =============================================================
// Settings
// =============================================================
template <class Func>
void Settings::ForEachINIOption(Settings& settings, Func&& option) {
	// System Settings
	option(settings.ED_DegradationDisabled, "Degradation", "DisableDegradation");
	option(settings.ED_BreakDisabled, "Degradation", "DisableBreak");
	option(settings.ED_AffectPlayer, "Degradation", "AffectPlayer");
	option(settings.ED_AffectFollower, "Degradation", "AffectFollower");
	option(settings.ED_AffectNPC, "Degradation", "AffectNPC");
	option(settings.ED_NoBreakNoEnchant, "Degradation", "NoBreakMagicDisallowEnchanting");
	option(settings.ED_RemoveEquipment, "Degradation", "RemoveBrokenEquipment");
	option(settings.ED_IncreasedDurability, "Degradation", "IncreaseDurability");
	option(settings.ED_Material_Multiplier, "Degradation", "MaterialMultiplier");
	option(settings.ED_BreakThreshold, "Degradation", "BreakThreshold");

	// Degradation Rates
	option(settings.ED_Degrade_Sword, "DegradationRate", "WeaponSword");
	option(settings.ED_Degrade_Dagger, "DegradationRate", "WeaponDagger");
	option(settings.ED_Degrade_WarAxe, "DegradationRate", "WeaponWarAxe");
	option(settings.ED_Degrade_Mace, "DegradationRate", "WeaponMace");
	option(settings.ED_Degrade_GreatSword, "DegradationRate", "WeaponGreatSword");
	option(settings.ED_Degrade_Warhammer, "DegradationRate", "WeaponHammer");
	option(settings.ED_Degrade_BattleAxe, "DegradationRate", "WeaponBattleAxe");
	option(settings.ED_Degrade_Bow, "DegradationRate", "WeaponBow");
	option(settings.ED_Degrade_CrossBow, "DegradationRate", "WeaponCrossbow");
	option(settings.ED_Degrade_LightArmor, "DegradationRate", "LightArmor");
	option(settings.ED_Degrade_HeavyArmor, "DegradationRate", "HeavyArmor");
	option(settings.ED_Degrade_Clothing, "DegradationRate", "Clothing");
	option(settings.ED_Degrade_Armor, "DegradationRate", "DefaultArmor");
	option(settings.ED_Degrade_PowerAttack, "DegradationRate", "PowerAttackMultiplier");
	option(settings.ED_Degrade_FollowerMulti, "DegradationRate", "FollowerMultiplier");
	option(settings.ED_Degrade_NPCMulti, "DegradationRate", "NPCMultiplier");

	// Break Rates
	option(settings.ED_Break_Sword, "BreakChance", "WeaponSword");
	option(settings.ED_Break_Dagger, "BreakChance", "WeaponDagger");
	option(settings.ED_Break_WarAxe, "BreakChance", "WeaponWarAxe");
	option(settings.ED_Break_Mace, "BreakChance", "WeaponMace");
	option(settings.ED_Break_GreatSword, "BreakChance", "WeaponGreatSword");
	option(settings.ED_Break_Warhammer, "BreakChance", "WeaponHammer");
	option(settings.ED_Break_BattleAxe, "BreakChance", "WeaponBattleAxe");
	option(settings.ED_Break_Bow, "BreakChance", "WeaponBow");
	option(settings.ED_Break_CrossBow, "BreakChance", "WeaponCrossbow");
	option(settings.ED_Break_LightArmor, "BreakChance", "LightArmor");
	option(settings.ED_Break_HeavyArmor, "BreakChance", "HeavyArmor");
	option(settings.ED_Break_Clothing, "BreakChance", "Clothing");
	option(settings.ED_Break_Armor, "BreakChance", "DefaultArmor");
	option(settings.ED_Break_PowerAttack, "BreakChance", "PowerAttackMultiplier");
	option(settings.ED_Break_FollowerMulti, "BreakChance", "FollowerMultiplier");
	option(settings.ED_Break_NPCMulti, "BreakChance", "NPCMultiplier");

	// Skill Rates
	option(settings.ED_Skill_Enabled, "SkillMultiplier", "SkillEnabled");
	option(settings.ED_Skill_Rate, "SkillMultiplier", "SkillRate");

	// Material Rates
	option(settings.ED_Daedric, "MaterialMultiplier", "Daedric");
	option(settings.ED_Dragon, "MaterialMultiplier", "Dragon");
	option(settings.ED_Ebony, "MaterialMultiplier", "Ebony");
	option(settings.ED_Stalhrim, "MaterialMultiplier", "Stalhrim");
	option(settings.ED_Orcish, "MaterialMultiplier", "Orcish");
	option(settings.ED_Glass, "MaterialMultiplier", "Glass");
	option(settings.ED_Dwarven, "MaterialMultiplier", "Dwarven");
	option(settings.ED_Chitin, "MaterialMultiplier", "Chitin");
	option(settings.ED_Falmer, "MaterialMultiplier", "Falmer");
	option(settings.ED_Bonemold, "MaterialMultiplier", "Bonemold");
	option(settings.ED_Elven, "MaterialMultiplier", "Elven");
	option(settings.ED_Silver, "MaterialMultiplier", "Silver");
	option(settings.ED_Steel, "MaterialMultiplier", "Steel");
	option(settings.ED_Iron, "MaterialMultiplier", "Iron");
	option(settings.ED_Leather, "MaterialMultiplier", "Leather");
	option(settings.ED_Fur, "MaterialMultiplier", "Fur");

	// Dynamic Temper
	option(settings.ED_Temper_Enabled, "DynamicTemper", "EnableDynamicTemper");
	option(settings.ED_Temper_Chance, "DynamicTemper", "TemperChance");
	option(settings.ED_Temper_VendorChance, "DynamicTemper", "VendorTemperChance");
	option(settings.ED_Temper_BossChance, "DynamicTemper", "BossTemperChance");

	// Dynamic Enchanting
	option(settings.ED_Enchant_Enabled, "DynamicEnchant", "EnableDynamicEnchant");
	option(settings.ED_Enchant_Chance, "DynamicEnchant", "EnchantChance");
	option(settings.ED_Enchant_VendorChance, "DynamicEnchant", "VendorEnchantChance");
	option(settings.ED_Enchant_BossChance, "DynamicEnchant", "BossEnchantChance");

	// Widget
	option(settings.ED_Widget_Display, "Widget", "DisplayType");
	option(settings.ED_Widget_Layout, "Widget", "Layout");
	option(settings.ED_Widget_PosX, "Widget", "PositionX");
	option(settings.ED_Widget_PosY, "Widget", "PositionY");
	option(settings.ED_Widget_Scale, "Widget", "Scale");
	option(settings.ED_Color_Unbreakable, "Widget", "UnbreakableColor");
	option(settings.ED_Color_Broken, "Widget", "BreakableColor");
	option(settings.ED_Widget_ToggleKeyCode, "Widget", "ToggleKeyCode");
	option(settings.ED_Widget_ToggleDuration, "Widget", "ToggleDuration");
	option(settings.ED_Widget_Reverse, "Widget", "Reverse");
	option(settings.ED_Widget_ShowPoisonName, "Widget", "ShowPoisonName");
	option(settings.ED_Widget_ShowShout, "Widget", "ShowShout");
	option(settings.ED_Widget_ShowWeaponName, "Widget", "ShowWeaponName");
	option(settings.ED_Widget_ShowArmorName, "Widget", "ShowArmorName");
	option(settings.ED_Widget_ShowHealth, "Widget", "ShowHealth");
	option(settings.ED_Widget_ShowUnarmed, "Widget", "ShowUnarmed");

	// Temper Name Settings
	option(settings.ED_Names_Style, "Temper Names", "DisplayStyle");
}

Settings* Settings::GetSingleton() {
	static Settings singleton;
	return std::addressof(singleton);
}

void Settings::LoadINI() {
	CSimpleIniA iniSettings;
	iniSettings.SetUnicode();
	SI_Error iniError = iniSettings.LoadFile(setting_path);

	ForEachINIOption(*this, [this, &iniSettings](auto& value, const char* section, const char* key) {
		get_value(iniSettings, value, section, key);
	});

	// Temper names that need whitespace conversion.
	get_value(iniSettings, ED_Names_Broken, "Temper Names", "Broken");
	get_value(iniSettings, ED_Names_Prefix, "Temper Names", "Prefix");
	get_value(iniSettings, ED_Names_Postfix, "Temper Names", "Postfix");
	get_value(iniSettings, ED_Names_CustomNames, "Temper Names", "CustomNames");

	// Replace underscores with a white space in all entries
	std::replace(ED_Names_Broken.begin(), ED_Names_Broken.end(), '_', ' ');
	std::replace(ED_Names_Prefix.begin(), ED_Names_Prefix.end(), '_', ' '); 
	std::replace(ED_Names_Postfix.begin(), ED_Names_Postfix.end(), '_', ' '); 
	std::replace(ED_Names_CustomNames.begin(), ED_Names_CustomNames.end(), '_', ' '); 

	// Get the list of custom names
	CustomNames = SplitString(ED_Names_CustomNames);

	// Set the material map
	MaterialRates = {
		{GameData::Material::Daedric,  ED_Daedric},
		{GameData::Material::Dragon,   ED_Dragon},
		{GameData::Material::Ebony,    ED_Ebony},
		{GameData::Material::Stalhrim, ED_Stalhrim},
		{GameData::Material::Orcish,   ED_Orcish},
		{GameData::Material::Glass,    ED_Glass},
		{GameData::Material::Dwarven,  ED_Dwarven},
		{GameData::Material::Chitin,   ED_Chitin},
		{GameData::Material::Falmer,   ED_Falmer},
		{GameData::Material::Bonemold, ED_Bonemold},
		{GameData::Material::Elven,    ED_Elven},
		{GameData::Material::Silver,   ED_Silver},
		{GameData::Material::Steel,    ED_Steel},
		{GameData::Material::Iron,     ED_Iron},
		{GameData::Material::Leather,  ED_Leather},
		{GameData::Material::Fur,      ED_Fur}
	};

	// Process the rest of the lists
	ProcessNoBreakForms();
	ProcessEnchantingForms();
	ProcessMaterialForms();
	SetVendorList();

	// If not found, create it
	if (iniError < 0)
		iniSettings.SaveFile(setting_path);
}

void Settings::SaveINI() {
	CSimpleIniA iniSettings;
	iniSettings.SetUnicode();
	iniSettings.LoadFile(setting_path);

	ForEachINIOption(*this, [this, &iniSettings](const auto& value, const char* section, const char* key) {
		set_value(iniSettings, value, section, key);
	});

	// Replace whitespaces with underscores before saving
	std::string Names_Broken = [] (std::string s) { std::replace(s.begin(), s.end(), ' ', '_'); return s; }(ED_Names_Broken);
	std::string Names_Prefix = [] (std::string s) { std::replace(s.begin(), s.end(), ' ', '_'); return s; }(ED_Names_Prefix);
	std::string Names_Postfix = [] (std::string s) { std::replace(s.begin(), s.end(), ' ', '_'); return s; }(ED_Names_Postfix);

	// Temper Name Settings
	set_value(iniSettings, Names_Broken, "Temper Names", "Broken");
	set_value(iniSettings, Names_Prefix, "Temper Names", "Prefix");
	set_value(iniSettings, Names_Postfix, "Temper Names", "Postfix");

	// Special case, join strings before saving
	ED_Names_CustomNames = JoinStrings(CustomNames);
	set_value(iniSettings, ED_Names_CustomNames, "Temper Names", "CustomNames");

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
		std::vector<std::string> parts = SplitString(kv.pItem);

		// Guard against invalid entries
		if (parts.size() < 2) {
			logger::warn("Invalid break entry: {}", kv.pItem);
			continue;
		}

		// Process the no break form
		if (auto* form = RE::TESDataHandler::GetSingleton()->LookupForm(RE::FormID(std::stoull(parts[1], nullptr, 0)), parts[0]))
			noBreakForms.insert(form->formID);
	}

	logger::info("Loaded: {} No Break Forms", noBreakForms.size());
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
		// 0 - ESP Name, 1 - FormID, 2 - Body Part, 3 - Tier, 4 - Enchant Suffix
		std::vector<std::string> parts = SplitString(kv.pItem);

		// Guard against invalid entries
		if (parts.size() < 5) {
			logger::warn("Invalid material entry: {}", kv.pItem);
			continue;
		}

		// Process the enchantment
		if (auto* newItem = RE::TESDataHandler::GetSingleton()->LookupForm(RE::FormID(std::stoull(parts[1], nullptr, 0)), parts[0])) {
			if (!newItem) continue;

			// Is the enchantment valid
			auto* enchantItem = newItem->As<RE::EnchantmentItem>();
			if (!enchantItem) continue; // not an enchantment

			// Setup the enchantment
			GameData::Enchantment newEnchantment ;
			newEnchantment.enchantment = enchantItem;
			newEnchantment.tier = std::stoi(parts[3]);
			newEnchantment.suffix = parts[4];

			// Split body parts by comma in case of multiple entries
			std::vector<std::string> bodyParts = SplitString(parts[2], ",");

			// Add the enchantment to the correct array based on the given body parts
			for (auto& bp : bodyParts) {
				std::string lowerBP = bp;
				std::transform(lowerBP.begin(), lowerBP.end(), lowerBP.begin(), ::tolower);

				if (lowerBP == "weapon") enchantWeapon.push_back(newEnchantment);
				else if (lowerBP == "head") enchantHead.push_back(newEnchantment);
				else if (lowerBP == "body") enchantBody.push_back(newEnchantment);
				else if (lowerBP == "hand") enchantHand.push_back(newEnchantment);
				else if (lowerBP == "foot") enchantFoot.push_back(newEnchantment);
				else if (lowerBP == "shield") enchantShield.push_back(newEnchantment);
			}
		}
	}

	logger::info("Loaded: {} Weapon Enchantments", enchantWeapon.size());
	logger::info("Loaded: {} Head Enchantments", enchantHead.size());
	logger::info("Loaded: {} Body Enchantments", enchantBody.size());
	logger::info("Loaded: {} Hand Enchantments", enchantHand.size());
	logger::info("Loaded: {} Foot Enchantments", enchantFoot.size());
	logger::info("Loaded: {} Shield Enchantments", enchantShield.size());
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
		std::vector<std::string> parts = SplitString(kv.pItem);

		// Guard against invalid entries
		if (parts.size() < 3) {
			logger::warn("Invalid material entry: {}", kv.pItem);
			continue;
		}

		// Get the associated material
		auto it = GameData::StringToMaterial.find(parts[2]);
		if (it != GameData::StringToMaterial.end()) {
			// Get the form ID and add it to our list of materials
			if (auto* newItem = RE::TESDataHandler::GetSingleton()->LookupForm(RE::FormID(std::stoull(parts[1], nullptr, 0)), parts[0])) {
				materialMap.emplace(newItem->formID, it->second);
			}
		}
	}
}

// =============================================================
// Public Functions
// =============================================================
// Break and Degradation Rates
double Settings::GetDegradationRate(RE::TESForm* form, RE::Actor* a_actor) {
	auto utility = Utility::GetSingleton();

	// Get Skill Rate Factor
	double skillMulti = 1.0f;
	if (a_actor) skillMulti = SkillRate(a_actor, form);

	// Weapon Degradation
	if (auto* weap = form->As<RE::TESObjectWEAP>()) {
		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Degradation Rate
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword: return ED_Degrade_Sword * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandDagger: return ED_Degrade_Dagger * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandAxe: return ED_Degrade_WarAxe * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandMace: return ED_Degrade_Mace * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kTwoHandSword: return ED_Degrade_GreatSword * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer)) return ED_Degrade_Warhammer * matMulti * skillMulti;
				else return ED_Degrade_BattleAxe * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kBow: return ED_Degrade_Bow * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kCrossbow: return ED_Degrade_CrossBow * matMulti * skillMulti;
			default: return ED_Degrade_Sword * matMulti * skillMulti;
		}

	// Armor Degradation
	} else if (auto* armor = form->As<RE::TESObjectARMO>()) {
		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Degradation Rate
		if (armor->IsLightArmor()) return ED_Degrade_LightArmor * matMulti * skillMulti;
		else if (armor->IsHeavyArmor()) return ED_Degrade_HeavyArmor * matMulti * skillMulti;
		else if (armor->HasKeyword(utility->keywordClothing)) return ED_Degrade_Clothing * matMulti * skillMulti;
	}

	return ED_Degrade_Armor;
}

double Settings::GetBreakChance(RE::TESForm* form, RE::Actor* a_actor) {
	auto utility = Utility::GetSingleton();

	// Get Skill Rate Factor
	double skillMulti = 1.0f;
	if (a_actor) skillMulti = SkillRate(a_actor, form);

	if (auto* weap = form->As<RE::TESObjectWEAP>()) {
		// Material Check
		double matMulti = MaterialRate(weap->GetKeywords());

		// Default Break Chance
		switch (weap->GetWeaponType()) {
			case RE::WEAPON_TYPE::kOneHandSword: return ED_Break_Sword * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandDagger: return ED_Break_Dagger * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandAxe: return ED_Break_WarAxe * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kOneHandMace: return ED_Break_Mace * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kTwoHandSword: return ED_Break_GreatSword * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kTwoHandAxe:
				if (weap->HasKeyword(utility->keywordWarhammer)) return ED_Break_Warhammer * matMulti * skillMulti;
				else return ED_Break_BattleAxe * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kBow: return ED_Break_Bow * matMulti * skillMulti;
			case RE::WEAPON_TYPE::kCrossbow: return ED_Break_CrossBow * matMulti * skillMulti;
			default: return ED_Break_Sword * matMulti * skillMulti;
		}
	} else if (auto* armor = form->As<RE::TESObjectARMO>()) {
		// Material Check
		double matMulti = MaterialRate(armor->GetKeywords());

		// Default Break Chance
		if (armor->IsLightArmor()) return ED_Break_LightArmor * matMulti * skillMulti;
		else if (armor->IsHeavyArmor()) return ED_Break_HeavyArmor * matMulti * skillMulti;
		else if (armor->HasKeyword(utility->keywordClothing)) return ED_Break_Clothing * matMulti * skillMulti;
	}

	return ED_Break_Armor;
}

// Enchant List Functions
std::vector<GameData::Enchantment>* Settings::GetEnchantmentList(EquipmentType type) {
	// Select correct vector based on part
    switch (type) {
    	case EquipmentType::Weapon: return &enchantWeapon;
    	case EquipmentType::Head:   return &enchantHead;
    	case EquipmentType::Body:   return &enchantBody;
    	case EquipmentType::Hand:   return &enchantHand;
    	case EquipmentType::Foot:   return &enchantFoot;
    	case EquipmentType::Shield: return &enchantShield;
    }

	return nullptr;
}

// Break Form Functions
bool Settings::HasNoBreakForms(int formid) {
	return noBreakForms.contains(formid);
}

// Vendor Container Functions
bool Settings::IsVendorContainer(RE::TESObjectREFR* form) {
	return vendorContainers.find(form) != vendorContainers.end();
}

void Settings::SetVendorList() {
	// Get all factions
	RE::BSTArray<RE::TESForm *> factionList = RE::TESDataHandler::GetSingleton()->GetFormArray(RE::FormType::Faction);

	// Loop through all found factions
	for (RE::TESForm * faction : factionList) {
		RE::TESFaction* foundFaction = faction->As<RE::TESFaction>();

		if (foundFaction->IsVendor() && foundFaction->vendorData.merchantContainer)
			vendorContainers.insert(foundFaction->vendorData.merchantContainer);
	}

	logger::debug("Loaded: {} Vendor Containers", vendorContainers.size());
}

// =============================================================
// Private Functions
// =============================================================
// Material Setting
double Settings::MaterialRate(std::span<RE::BGSKeyword*> keywords) {
	int iKeywordCount = 0;
	double Rates = 0;

	// If the material system is turned off
	if (!ED_Material_Multiplier) 
		return 1.0;

	// If Material Multiplier is on
	for (RE::BGSKeyword* keyword : keywords) {
		if (!keyword) continue;
		RE::FormID tmp = keyword->GetFormID();
		if (auto it = materialMap.find(tmp); it != materialMap.end()) {
			Rates += MaterialRates.at(it->second);
			++iKeywordCount;
		}
	}
	// If no keywords, move on
	if (iKeywordCount == 0) return 1.0f;

	// Get the average rate and return it
	double AverageRate = Rates / iKeywordCount;
	double FixedRate = AverageRate / 100.0;
	return 1.0f - FixedRate;
	
}

// Get the relavent skill based on the object
RE::ActorValue Settings::GetRelevantSkill(RE::TESForm* form) {
	RE::ActorValue av = RE::ActorValue::kNone;

	// Weapon Value
	if (auto* weap = form->As<RE::TESObjectWEAP>()) {
		auto WeaponType = weap->GetWeaponType();

		// Set the Value Type
		av = RE::ActorValue::kOneHanded;
		if (WeaponType == RE::WEAPON_TYPE::kTwoHandSword || WeaponType == RE::WEAPON_TYPE::kTwoHandAxe)
			av = RE::ActorValue::kTwoHanded;
		else if (WeaponType == RE::WEAPON_TYPE::kBow || WeaponType == RE::WEAPON_TYPE::kCrossbow)
			av = RE::ActorValue::kArchery;
	} else if (auto* armor = form->As<RE::TESObjectARMO>()) {
		if (armor->IsLightArmor()) 
			av = RE::ActorValue::kLightArmor;
		else if (armor->IsHeavyArmor())
			av = RE::ActorValue::kHeavyArmor;
	}

	return av;
}

// Skill Setting
double Settings::SkillRate(RE::Actor* a_actor, RE::TESForm* a_form) {
	if (!a_actor || !ED_Skill_Enabled) return 1.0f;

	// Get the relavent skill value
	RE::ActorValue a_value = GetRelevantSkill(a_form);
	if (a_value == RE::ActorValue::kNone) return 1.0f;

	// Get the mitigation factor
	float SkillRate = ED_Skill_Rate / 100.0f;
	float SkillValue = a_actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIERS::kTotal, a_value);
	float mitigation = Degredation::kMaxSkillMitigation * SkillValue / (SkillValue + SkillRate);

	// Return the mitigation
	return 1.0f - mitigation;
}

// =============================================================
// String Functions
// =============================================================
std::vector<std::string> Settings::SplitString(std::string_view str, std::string_view delimiter)  {
    std::vector<std::string> result;

    size_t start = 0;
    while (true) {
        size_t pos = str.find(delimiter, start);

        if (pos == std::string_view::npos) {
            result.emplace_back(str.substr(start));
            break;
        }

        result.emplace_back(str.substr(start, pos - start));
        start = pos + delimiter.size();
    }

    return result;
}

std::string Settings::JoinStrings(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << parts[i].c_str();
    }
    return oss.str();
}

// =============================================================
// INI Functions
// =============================================================
template <class T>
void Settings::set_value(CSimpleIniA& a_ini, const T& a_value, const char* a_section, const char* a_key)
{
	if constexpr (std::is_same_v<T, bool>) {
		a_ini.SetBoolValue(a_section, a_key, a_value);
	} else if constexpr (std::is_same_v<T, std::string>) {
		a_ini.SetValue(a_section, a_key, a_value.c_str());
	} else if constexpr (std::is_same_v<T, std::uint32_t>) {
		a_ini.SetValue(a_section, a_key, std::format("0x{:X}", a_value).c_str());
	} else {
		a_ini.SetValue(a_section, a_key, std::to_string(a_value).c_str());
	}
}

template <class T>
void Settings::get_value(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key)
{
	if (a_ini.KeyExists(a_section, a_key)) {
		if constexpr (std::is_same_v<T, bool>) {
			a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
		} else if constexpr (std::is_same_v<T, std::string>) {
			a_value = a_ini.GetValue(a_section, a_key, a_value.c_str());
		} else if constexpr (std::is_same_v<T, std::uint32_t>) {
			a_value = static_cast<std::uint32_t>(
				std::stoul(a_ini.GetValue(a_section, a_key, std::format("0x{:X}", a_value).c_str()), nullptr, 0)
			);
		} else if constexpr (std::is_integral_v<T>) {
			a_value = static_cast<T>(std::stoi(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str())));
		} else if constexpr (std::is_floating_point_v<T>) {
			a_value = static_cast<T>(std::stof(a_ini.GetValue(a_section, a_key, std::to_string(a_value).c_str())));
		}
	} else {
		set_value(a_ini, a_value, a_section, a_key);
	}
}
