#include <cmath>
#include <random>

namespace Degredation
{
    // Health Boundries
    inline constexpr float kMinHealth = 0.900f;
    inline constexpr float kMaxHealth = 1.000f;
    inline constexpr float kBrokenHealth = 0.100f;
    inline constexpr float kBrokenHealthThreshold = 0.500f;

    // Max Skill Mitigation
    constexpr float kMaxSkillMitigation = 0.40f;
    
    // Degradation Rate Variables
    constexpr double kMinLossAt100 = 0.0015;
    constexpr double kMaxLossAt100 = 0.0040;
    constexpr double kCurve = 1.5;
    constexpr double kPrecision = 10000.0;

    // Round the float value to the nearest tenth
    inline float CeilToTenths(float value)
    {
        constexpr double precision = 10.0;
        constexpr double epsilon = 0.000001;

        return static_cast<float>(
            std::ceil((static_cast<double>(value) * precision) - epsilon) / precision
        );
    }

    // Round to the desired precision
    template <class T>
    inline T RoundToPrecision(T value, double precision = kPrecision)
    {
        return static_cast<T>(std::round(static_cast<double>(value) * precision) / precision);
    }

    // Truncate value to the decimal selected
    template <class T>
    inline T TruncateToDecimals(T value, int decimals)
    {
        const double factor = std::pow(10.0, decimals);
        const double scaled = static_cast<double>(value) * factor;
        const double epsilon = 1e-6;
        const double truncated = std::trunc(scaled + (scaled >= 0.0 ? epsilon : -epsilon));

        return static_cast<T>(truncated / factor);
    }
    
}

namespace Random
{
    inline int Int(int min, int max)
    {
        thread_local std::mt19937 mt{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }

    inline double Double(double min, double max)
    {
        thread_local std::mt19937 mt{ std::random_device{}() };
        std::uniform_real_distribution<double> dist(min, max);
        return dist(mt);
    }
}

namespace Probability
{
    inline bool Int(int percentChance)
    {
        if (percentChance <= 0) return false;
        if (percentChance >= 100) return true;

        thread_local std::mt19937 mt{ std::random_device{}() }; 
        std::uniform_int_distribution<int> dist(0, 99);
        int roll = dist(mt);

        return roll < percentChance;
    }

    inline bool Double(double percentChance)
    {
    if (percentChance <= 0.0) return false;
    if (percentChance >= 100.0) return true;

	thread_local std::mt19937 mt{ std::random_device{}() }; 
	std::uniform_real_distribution<double> dist(0.0, 100.0);
    double roll = dist(mt);

    return roll < percentChance;
    }
}

enum class EquipmentType
{
    Weapon,
    Head,
    Body,
    Hand,
    Foot,
    Shield,
    None
};

// ===========================
// Material Enumerator
// ===========================
namespace GameData {

    enum class Material : uint8_t {
        Fur,
        Leather,
        Silver,
        Iron, 
        Steel,
        Dwarven,
        Orcish,
        Bonemold,
        Falmer,
        Elven,
        Glass,
        Nordic,
        Chitin, 
        Ebony,
        Stalhrim,
        Daedric,
        Dragon
    };

    struct TierInfo {
        int tier;
        int minLevel;
        int charge;
    };

    struct MaterialLimits {
        int minTier;
        int maxTier;
    };

    struct Enchantment {
        RE::EnchantmentItem* enchantment;   // Enchanmtment form
        int tier;                           // Tier level (e.g., 1, 2, 3...)
        std::string suffix;                 // Suffix name (e.g., "of Luck")
    };

	// 0 - tier, 1 - Minimum Level, 2 - Max Charge
    static const std::vector<TierInfo> TierTable = {
        {1,  1,  500},
        {2,  9,  1000},
        {3, 17,  1500},
        {4, 25,  2000},
        {5, 37,  2500},
        {6, 46,  3000}
    };

    static const std::unordered_map<Material, MaterialLimits> MaterialTable = {
        {Material::Fur,        {1, 2}}, // Very early-game, weakest armor
        {Material::Leather,    {1, 3}}, // Early light armor
        {Material::Silver,     {1, 3}}, // Early-game / specialty
        {Material::Iron,       {1, 3}}, // Early-medium armor
        {Material::Steel,      {1, 3}}, // Early-medium armor
        {Material::Orcish,     {2, 4}}, // Mid-tier heavy/light
        {Material::Dwarven,    {2, 4}}, // Mid-tier heavy
        {Material::Bonemold,   {3, 4}}, // Mid-tier, light-to-medium
        {Material::Falmer,     {3, 5}}, // Mid-to-high, usually found in caves/dungeons
        {Material::Elven,      {3, 5}}, // Mid-tier light
        {Material::Glass,      {3, 5}}, // Mid-tier light
        {Material::Nordic,     {3, 5}}, // Mid-tier heavy
        {Material::Chitin,     {3, 5}}, // Mid-tier exotic (Dragonborn DLC)
        {Material::Ebony,      {4, 6}}, // High-end heavy/light
        {Material::Daedric,    {4, 6}}, // End-game heavy
        {Material::Stalhrim,   {4, 6}}, // End-game light/mid
        {Material::Dragon,     {4, 6}}  // End-game heavy
    };

    // String -> Material mapping
	static const std::unordered_map<std::string, Material> StringToMaterial = {
		{"Fur", Material::Fur},
		{"Leather", Material::Leather},
		{"Silver", Material::Silver},
		{"Iron", Material::Iron},
		{"Steel", Material::Steel},
		{"Orcish", Material::Orcish},
		{"Dwarven", Material::Dwarven},
		{"Bonemold", Material::Bonemold},
		{"Falmer", Material::Falmer},
		{"Elven", Material::Elven},
		{"Glass", Material::Glass},
		{"Nordic", Material::Nordic},
		{"Chitin", Material::Chitin}, 
		{"Ebony", Material::Ebony},
		{"Daedric", Material::Daedric},
		{"Stalhrim", Material::Stalhrim},
		{"Dragon", Material::Dragon}
	};

    // Helper to convert string -> Material enum safely
    std::optional<Material> MaterialFromString(const std::string& name) {
        auto it = StringToMaterial.find(name);
        if (it != StringToMaterial.end())
            return it->second;
        return std::nullopt;
    }
}