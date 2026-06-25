namespace cons 
{
    inline constexpr float kMinHealth = 0.999f;
    inline constexpr float kMaxHealth = 1.099f;
    inline constexpr float kBrokenHealth = 0.5f;
    inline constexpr float kDisplayHealth = 1.1f;
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