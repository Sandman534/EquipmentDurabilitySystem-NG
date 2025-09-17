#pragma once

class TemperManager {
public:
	// String Options
	static std::string AsVanilla(std::uint32_t a_level, bool a_isWeapon);
	static std::string AsVanillaPlus(std::uint32_t a_level, bool a_isWeapon);
	static std::string AsPlusN(std::uint32_t a_level, bool a_isWeapon);
	static std::string AsInternal(std::uint32_t a_level, bool a_isWeapon);
	static std::string AsCustom(std::uint32_t a_level, bool a_isWeapon);
	static std::string AsRomanNumeral(std::uint32_t a_level, bool a_isWeapon);

	// Hook Functions
	static const char* GetTemperFactor(float a_factor, bool a_isWeapon);
	static void VFormat(RE::BSString* a_dst, const char* a_fmt, ...);
	static void sprintf_s(char* a_buffer, std::size_t a_buffSize, const char* a_fmt, ...);

	// Initialize
	static void Init();
	static void InitStrings();

private:
	// Store the Temper name
	class FormatterMap {
	public:
		FormatterMap();
		std::string operator()(std::uint32_t a_factor, bool a_isWeapon);

	private:
		std::unordered_map<std::string, std::function<std::string(std::uint32_t, bool)>> _map;
	};

	// Globals
	static std::set<std::string> _stringCache;
	static FormatterMap _formatterMap;
	static inline std::array<std::string, 6> weaponSettings{};
	static inline std::array<std::string, 6> armorSettings{};
};