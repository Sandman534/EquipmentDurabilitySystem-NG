#pragma once

namespace Serialization {
	static constexpr std::uint32_t ID = 'EDSM';
	static constexpr std::uint32_t kVersion = 1;
	
	void SaveCallback(SKSE::SerializationInterface* a_skse);
}