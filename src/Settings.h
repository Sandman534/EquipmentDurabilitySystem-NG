#pragma once

#include <unordered_map>
#include <unordered_set>
#include "SimpleIni.h"
#include "FoundEquipData.h"

// ===========================
// Settings
// ===========================
class Settings {
public:
	// System Settings
	bool ED_DegradationDisabled{ false };
	bool ED_BreakDisabled { true };
	bool ED_OnlyPlayer{ true };
	bool ED_NoBreakNoEnchant{ true };
	bool ED_IncreasedDurability{ false };
	bool ED_Material_Multiplier{ true };
	int ED_BreakThreshold{ 50 };

	// Degredaton Rates
	float ED_Degrade_Sword{ 3.0 };
	float ED_Degrade_Dagger{ 3.0 };
	float ED_Degrade_WarAxe{ 3.0 };
	float ED_Degrade_Mace{ 3.0 };
	float ED_Degrade_GreatSword{ 3.0 };
	float ED_Degrade_Warhammer{ 3.0 };
	float ED_Degrade_BattleAxe{ 3.0 };
	float ED_Degrade_Bow{ 3.0 };
	float ED_Degrade_CrossBow{ 3.0 };
	float ED_Degrade_LightArmor{ 3.0 };
	float ED_Degrade_HeavyArmor{ 3.0 };
	float ED_Degrade_Clothing{ 3.0 };
	float ED_Degrade_Armor{ 3.0 };
	float ED_Degrade_PowerAttack{ 2.0 };
	float ED_Degrade_FollowerMulti{ 1.0 };
	float ED_Degrade_NPCMulti{ 1.0 };

	// Break Rates
	int ED_Break_Sword{ 10 };
	int ED_Break_Dagger{ 10 };
	int ED_Break_WarAxe{ 10 };
	int ED_Break_Mace{ 10 };
	int ED_Break_GreatSword{ 10 };
	int ED_Break_Warhammer{ 10 };
	int ED_Break_BattleAxe{ 10 };
	int ED_Break_Bow{ 10 };
	int ED_Break_CrossBow{ 10 };
	int ED_Break_LightArmor{ 10 };
	int ED_Break_HeavyArmor{ 10 };
	int ED_Break_Clothing{ 10 };
	int ED_Break_Armor{ 10 };
	float ED_Break_PowerAttack{ 2.0 };
	float ED_Break_FollowerMulti{ 1.0 };
	float ED_Break_NPCMulti{ 1.0 };

	// Material Multiplier
	float ED_Daedric{ 0.40 };
	float ED_Dragon{ 0.44 };
	float ED_Ebony{ 0.48 };
	float ED_Stalhrim{ 0.52 };
	float ED_Orcish{ 0.56 };
	float ED_Glass{ 0.60 };
	float ED_Dwarven{ 0.64 };
	float ED_Chitin{ 0.68 };
	float ED_Falmer{ 0.72 };
	float ED_Bonemold{ 0.76 };
	float ED_Elven{ 0.80 };
	float ED_Silver{ 0.84 };
	float ED_Steel{ 0.88 };
	float ED_Iron{ 0.92 };
	float ED_Leather{ 0.96 };
	float ED_Fur{ 1.0 };

	// Dynamic Temper
	bool ED_Temper_Enabled{ true };
	int ED_Temper_Chance{ 40 };
	int ED_Temper_VendorChance{ 50 };
	int ED_Temper_BossChance{ 80 };

	// Dynamic Enchanting
	bool ED_Enchant_Enabled{ true };
	int ED_Enchant_Chance{ 10 };
	int ED_Enchant_VendorChance{ 15 };
	int ED_Enchant_BossChance{ 20 };

	// Widget Position and Color
	int ED_Widget_Display{ 1 };
	int ED_Widget_PosX{ 3 };
	int ED_Widget_PosY{ 73 };
	int ED_Widget_Scale{ 100 };
	std::uint32_t ED_Color_Unbreakable{ 0xFFFFFF };
	std::uint32_t ED_Color_Broken{ 0xFFD700 };

	// Widget Hotkeys
	int ED_Widget_ToggleKeyCode{ -1 };
	int ED_Widget_ToggleDuration{ 0 };

	// Widget Layout Options
	bool ED_Widget_Reverse{ false };
	bool ED_Widget_ShowPoisonName{ true };
	bool ED_Widget_ShowShout{ true };
	bool ED_Widget_ShowWeaponName{ true };
	bool ED_Widget_ShowArmorName{ true };
	bool ED_Widget_ShowHealth{ true };
	bool ED_Widget_ShowUnarmed{ true };

	// Processing
	void LoadINI();
	void SaveINI();
	void ProcessNoBreakForms();
	void ProcessEnchantingForms();
	void ProcessMaterialForms();

	// Enchantment Lists
	std::vector<GameData::Enchantment>* GetEnchantmentList(std::string part);

	// Material Data
	std::unordered_map<RE::FormID, GameData::Material> materialMap;

	// Checks
	bool IsVendorContainer(RE::TESObjectREFR* form);
	bool HasNoBreakForms(int formid);

	// Public Functions
	double GetDegradationRate(RE::TESForm* form);
	double GetBreakChance(RE::TESForm* form);

	static Settings* GetSingleton();
private:
    // cannot use auto for class member declaration
	static constexpr const wchar_t* setting_path = L"Data/SKSE/Plugins/EquipmentDurability/Settings.ini";
	static constexpr const wchar_t* list_path = L"Data/SKSE/Plugins/EquipmentDurability/EquipmentLists.ini";

	// No Break Forms
	std::unordered_set<int> noBreakForms;

	// Enchantment Lists
	std::vector<GameData::Enchantment> enchantWeapon;
	std::vector<GameData::Enchantment> enchantBody;
	std::vector<GameData::Enchantment> enchantHead;
	std::vector<GameData::Enchantment> enchantHand;
	std::vector<GameData::Enchantment> enchantFoot;
	std::vector<GameData::Enchantment> enchantShield;

	// Material Multipliers
	std::unordered_map<GameData::Material, double> MaterialRates;
	double MaterialRate(std::span<RE::BGSKeyword*> keywords);

	// Vendor Chests
	std::vector<RE::TESObjectREFR*> vendorContainers;
	void SetVendorList();

	// String Functions
	std::vector<std::string> SplitString(const std::string& s, char delimiter);

	// Get Values
	static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key);
	static void get_value(CSimpleIniA& a_ini, std::string& a_value, const char* a_section, const char* a_key);
	static void get_value(CSimpleIniA& a_ini, int& a_value, const char* a_section, const char* a_key);
	static void get_value(CSimpleIniA& a_ini, float& a_value, const char* a_section, const char* a_key);
	static void get_value(CSimpleIniA& a_ini, std::uint32_t& a_value, const char* a_section, const char* a_key);

	// Set Values
	static void set_value(CSimpleIniA& a_ini, bool a_value, const char* a_section, const char* a_key);
	static void set_value(CSimpleIniA& a_ini, std::string a_value, const char* a_section, const char* a_key);
	static void set_value(CSimpleIniA& a_ini, int a_value, const char* a_section, const char* a_key);
	static void set_value(CSimpleIniA& a_ini, float a_value, const char* a_section, const char* a_key);
	static void set_value(CSimpleIniA& a_ini, std::uint32_t a_value, const char* a_section, const char* a_key);

};