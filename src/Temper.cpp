#include "Temper.h"
#include "Settings.h"

std::string TemperManager::AsVanilla(std::uint32_t a_level, bool a_isWeapon) {
    // clamp level to 1–6
    if (a_level < 1) a_level = 1;
    if (a_level > 6) a_level = 6;

	// Get the Weapon or Armor array
    const auto& arr = a_isWeapon ? weaponSettings : armorSettings;
    return arr[a_level - 1]; // arrays are 0-indexed
}

std::string TemperManager::AsVanillaPlus(std::uint32_t a_level, bool a_isWeapon) {
	auto result = AsVanilla(a_level, a_isWeapon);
	if (a_level > 5) {
		result += " ";
		result += std::to_string(a_level - 5);
	}
	return result;
}

std::string TemperManager::AsPlusN(std::uint32_t a_level, [[maybe_unused]] bool a_isWeapon) {
	return std::string("+") + std::to_string(a_level);
}

std::string TemperManager::AsInternal(std::uint32_t a_level, [[maybe_unused]] bool a_isWeapon) {
	std::string result = std::to_string((a_level / 10) + 1);
	result += ".";
	result += std::to_string(a_level % 10);
	return result;
}

std::string TemperManager::AsCustom(std::uint32_t a_level, [[maybe_unused]] bool a_isWeapon) {
	std::size_t idx = a_level - 1;
	if (idx < Settings::GetSingleton()->CustomNames.size()) {
		return (Settings::GetSingleton()->CustomNames)[idx];
	} else {
		return Settings::GetSingleton()->CustomNames.empty() ? "" : Settings::GetSingleton()->CustomNames.back();
	}
}

std::string TemperManager::AsRomanNumeral(std::uint32_t a_level, [[maybe_unused]] bool a_isWeapon) {
	constexpr std::pair<std::uint32_t, std::string_view> MILESTONES[] = {
		{ 1, "I" },
		{ 4, "IV" },
		{ 5, "V" },
		{ 9, "IX" },
		{ 10, "X" },
		{ 40, "XL" },
		{ 50, "L" },
		{ 100, "C" },
		{ 400, "CD" },
		{ 500, "D" },
		{ 900, "CM" },
		{ 1000, "M" }
	};

	constexpr std::size_t SIZE = std::extent<decltype(MILESTONES)>::value;

	std::string result;
	for (auto i = SIZE; a_level; --i) {
		auto div = a_level / MILESTONES[i].first;
		a_level = a_level % MILESTONES[i].first;
		while (div--) {
			result += MILESTONES[i].second;
		}
	}

	return result;
}

const char* TemperManager::GetTemperFactor(float a_factor, bool a_isWeapon) {
	auto fLevel = std::floor((a_factor - 1.0f) * 10.0f);

	// If the break system is enabled, and we are below 0, return the broken name
	if (a_factor < 0.5f && !Settings::GetSingleton()->ED_BreakDisabled)
		return Settings::GetSingleton()->ED_Names_Broken.c_str();
	else if (a_factor < 1.1f)
		return 0;

	// Return whatever temper level we should be showing
	auto level = static_cast<std::uint32_t>(fLevel);
	auto it = _stringCache.insert(_formatterMap(level, a_isWeapon));
	return it.first != _stringCache.end() ? it.first->c_str() : 0;
}

void TemperManager::VFormat(RE::BSString* a_dst, const char* a_fmt, ...) {
	std::string fmt = "%s";
	fmt += Settings::GetSingleton()->ED_Names_Prefix;
	fmt += "%s";
	fmt += Settings::GetSingleton()->ED_Names_Postfix;

	std::va_list args1;
	va_start(args1, a_fmt);
	std::va_list args2;
	va_copy(args2, args1);
	std::vector<char> buf(std::vsnprintf(0, 0, fmt.c_str(), args1) + 1);
	va_end(args1);
	std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args2);
	va_end(args2);

	std::string_view view(buf.data(), buf.size());
	*a_dst = view;
}

void TemperManager::sprintf_s(char* a_buffer, std::size_t a_buffSize, const char* a_fmt, ...) {
	std::string fmt = Settings::GetSingleton()->ED_Names_Prefix;
	fmt += "%s";
	fmt += Settings::GetSingleton()->ED_Names_Postfix;

	std::va_list args;
	va_start(args, a_fmt);
	std::vsnprintf(a_buffer, a_buffSize, fmt.c_str(), args);
	va_end(args);
}

void TemperManager::Init() {
	// Relocations for SSE:AE
	REL::Relocation<std::uintptr_t> Base_Hook{ REL::RelocationID(12633, 12775), REL::Relocate(0x0 ,0x149) };
	REL::Relocation<std::uintptr_t> TemperFactor_Hook{ REL::RelocationID(12633,12775), REL::Relocate(0x59, 0x55) };
	REL::Relocation<std::uintptr_t> TemporFormat01_Hook{ REL::RelocationID(12633,12775), REL::Relocate(0x1A6, 0x25C) };
	REL::Relocation<std::uintptr_t> TemporFormat02_Hook{ REL::RelocationID(12633,12775), REL::Relocate(0x177, 0x22D) };
	REL::Relocation<std::uintptr_t> TemporString_Hook{ REL::RelocationID(12633,12775), REL::Relocate(0x13D, 0x1F6) };

	// This is an AE specific hook
	struct TemperFactorHook_Code : Xbyak::CodeGenerator {
		TemperFactorHook_Code(std::uintptr_t GetTemperAddr, std::uintptr_t returnAddr) {
			Xbyak::Label functLabel;
			Xbyak::Label retnLabel;

			movzx(edx, r9b);
			movaps(xmm0, xmm3);

			sub(rsp, 0x20);
			call(ptr[rip + functLabel]);
			add(rsp, 0x20);

			mov(rbx, rax);
			jmp(ptr[rip+retnLabel]);

			L(functLabel);
			dq(GetTemperAddr);

			L(retnLabel);
			dq(returnAddr);
		}
	};
	TemperFactorHook_Code code(reinterpret_cast<std::uintptr_t>(GetTemperFactor), Base_Hook.address());

	// AE and SSE have different functions
	auto& trampoline = SKSE::GetTrampoline();
	if (REL::Module::IsAE())
		trampoline.write_branch<6>(TemperFactor_Hook.address(), trampoline.allocate(code));
	else 
		trampoline.write_call<5>(TemperFactor_Hook.address(), TemperManager::GetTemperFactor);

	// These however are all the same
	trampoline.write_call<5>(TemporFormat01_Hook.address(), TemperManager::VFormat);
	trampoline.write_call<5>(TemporFormat02_Hook.address(), TemperManager::VFormat);
	trampoline.write_call<5>(TemporString_Hook.address(), TemperManager::sprintf_s);

	logger::info("Hook Installed: Temper Names");

	// Populate Weapon and Armor text for Vanilla
	InitStrings();
	logger::info("Loaded: Temper Names");
}

void TemperManager::InitStrings() {
    constexpr std::array<const char*, 6> weaponNames = {
        "sHealthDataPrefixWeap1",
        "sHealthDataPrefixWeap2",
        "sHealthDataPrefixWeap3",
        "sHealthDataPrefixWeap4",
        "sHealthDataPrefixWeap5",
        "sHealthDataPrefixWeap6"
    };

    constexpr std::array<const char*, 6> armorNames = {
        "sHealthDataPrefixArmo1",
        "sHealthDataPrefixArmo2",
        "sHealthDataPrefixArmo3",
        "sHealthDataPrefixArmo4",
        "sHealthDataPrefixArmo5",
        "sHealthDataPrefixArmo6"
    };

    auto gmst = RE::GameSettingCollection::GetSingleton();

	// Store Weapon and Armor strings
    for (std::size_t i = 0; i < weaponNames.size(); ++i) {
        if (auto setting = gmst->GetSetting(weaponNames[i])) {
            weaponSettings[i] = setting->GetString();
        }
        if (auto setting = gmst->GetSetting(armorNames[i])) {
            armorSettings[i] = setting->GetString();
        }
    }
}

TemperManager::FormatterMap::FormatterMap() : _map() {
	_map.insert({ "Vanilla", TemperManager::AsVanilla });
	_map.insert({ "VanillaPlus", TemperManager::AsVanillaPlus });
	_map.insert({ "PlusN", TemperManager::AsPlusN });
	_map.insert({ "Internal", TemperManager::AsInternal });
	_map.insert({ "Custom", TemperManager::AsCustom });
	_map.insert({ "RomanNumeral", TemperManager::AsRomanNumeral });
}

std::string TemperManager::FormatterMap::operator()(std::uint32_t a_factor, bool a_isWeapon) {
	auto it = _map.find(Settings::GetSingleton()->ED_Names_Style);
	if (it != _map.end())
		return it->second(a_factor, a_isWeapon);
	else
		return TemperManager::AsVanilla(a_factor, a_isWeapon);
}

decltype(TemperManager::_stringCache) TemperManager::_stringCache;
decltype(TemperManager::_formatterMap) TemperManager::_formatterMap;