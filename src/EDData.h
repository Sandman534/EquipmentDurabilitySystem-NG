#include <cmath>
#include <random>

namespace Degredation
{
    // Health Boundries
    inline constexpr float kMinHealth = 0.900f;
    inline constexpr float kMaxHealth = 1.000f;
    inline constexpr float kBrokenHealth = 0.5f;

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
