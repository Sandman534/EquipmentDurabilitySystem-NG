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

	bool ED_AffectPlayer{ true };
	bool ED_AffectFollower{ true };
	bool ED_AffectNPC{ true };

	bool ED_NoBreakNoEnchant{ true };
	bool ED_RemoveEquipment{ false };
	bool ED_IncreasedDurability{ false };
	bool ED_Material_Multiplier{ true };
	int ED_BreakThreshold{ 50 };

	// Degredaton Rates
	int ED_Degrade_Sword{ 25 };
	int ED_Degrade_Dagger{ 25 };
	int ED_Degrade_WarAxe{ 25 };
	int ED_Degrade_Mace{ 25 };
	int ED_Degrade_GreatSword{ 25 };
	int ED_Degrade_Warhammer{ 25 };
	int ED_Degrade_BattleAxe{ 25 };
	int ED_Degrade_Bow{ 25 };
	int ED_Degrade_CrossBow{ 25 };
	int ED_Degrade_LightArmor{ 25 };
	int ED_Degrade_HeavyArmor{ 25 };
	int ED_Degrade_Clothing{ 25 };
	int ED_Degrade_Armor{ 25 };
	int ED_Degrade_PowerAttack{ 50 };
	int ED_Degrade_FollowerMulti{ 0 };
	int ED_Degrade_NPCMulti{ 0 };

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
	int ED_Break_PowerAttack{ 50 };
	int ED_Break_FollowerMulti{ 0 };
	int ED_Break_NPCMulti{ 0 };

	// Material Multiplier
	int ED_Daedric{ 60 };
	int ED_Dragon{ 56 };
	int ED_Ebony{ 52 };
	int ED_Stalhrim{ 48 };
	int ED_Orcish{ 44 };
	int ED_Glass{ 40 };
	int ED_Dwarven{ 36 };
	int ED_Chitin{ 32 };
	int ED_Falmer{ 28 };
	int ED_Bonemold{ 24 };
	int ED_Elven{ 20 };
	int ED_Silver{ 16 };
	int ED_Steel{ 12 };
	int ED_Iron{ 8 };
	int ED_Leather{ 4 };
	int ED_Fur{ 0 };

	// Skill Multiplier
	bool ED_Skill_Enabled{ true };
	int ED_Skill_Rate{ 100 };

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
	int ED_Widget_Layout{ 0 };
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

	// Temper Names
	std::string ED_Names_Broken{ "Broken" };
	int ED_Names_Style{ 0 };
	std::string ED_Names_Prefix{ "(" };
	std::string ED_Names_Postfix{ ")" };
	std::string ED_Names_CustomNames{ "Okay|Decent|Good|Great|Awesome|Brilliant|Masterful|Wonderful|Astonishing|Amazing|Incredible|Unbelievable|Unimaginable|Mind-Boggling|Impossible|Infinite" };
	std::vector<std::string> ED_Names_StyleOptions = { "Vanilla", "VanillaPlus", "PlusN", "Internal", "Custom", "RomanNumeral", "Health" };

	// Custom Names
	std::vector<std::string> CustomNames;

	// Processing
	void LoadINI();
	void SaveINI();
	void ProcessNoBreakForms();
	void ProcessEnchantingForms();
	void ProcessMaterialForms();

	// Enchantment Lists
	std::vector<GameData::Enchantment>* GetEnchantmentList(EquipmentType type);

	// Material Data
	std::unordered_map<RE::FormID, GameData::Material> materialMap;

	// Checks
	bool IsVendorContainer(RE::TESObjectREFR* form);
	bool HasNoBreakForms(int formid);

	// Public Functions
	double GetDegradationRate(RE::TESForm* forms, RE::Actor* a_actor = nullptr);
	double GetBreakChance(RE::TESForm* form, RE::Actor* a_actor = nullptr);

	static Settings* GetSingleton();
private:
    // cannot use auto for class member declaration
	static constexpr const wchar_t* setting_path = L"Data/SKSE/Plugins/EquipmentDurability/Settings.ini";
	static constexpr const wchar_t* list_path = L"Data/SKSE/Plugins/EquipmentDurability/EquipmentLists.ini";

	template <class Func>
	void ForEachINIOption(Settings& settings, Func&& option);

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
	RE::ActorValue GetRelevantSkill(RE::TESForm* form);
	double SkillRate(RE::Actor* a_actor, RE::TESForm* a_form);

	// Vendor Chests
	std::unordered_set<RE::TESObjectREFR*> vendorContainers;
	void SetVendorList();

	// String Functions
	// std::vector<std::string> SplitString(const std::string& s, const std::string& delimiter = "|");
	std::vector<std::string> SplitString(std::string_view str, std::string_view delimiter = "|");
	std::string JoinStrings(const std::vector<std::string>& parts, const std::string& delimiter = "|");

	// Value Setter/Getter
	template <class T>
	void set_value(CSimpleIniA& a_ini, const T& a_value, const char* a_section, const char* a_key);

	template <class T>
	void get_value(CSimpleIniA& a_ini, T& a_value, const char* a_section, const char* a_key);
};