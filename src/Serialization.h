#pragma once

namespace Serialization {
	static constexpr std::uint32_t ID = 'EDSM';
	static constexpr std::uint32_t kVersion = 1;
	static constexpr std::uint32_t kContainers = 'CONT';
	static constexpr std::uint32_t kNPCs = 'NPCL';
	static constexpr std::uint32_t kState = 'BOOL';
	

	void SaveCallback(SKSE::SerializationInterface* a_skse);
	void LoadCallback(SKSE::SerializationInterface* a_skse);
	void RevertCallback(SKSE::SerializationInterface* a_skse);
}