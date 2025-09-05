#pragma once



namespace Serialization {
	static constexpr std::uint32_t SerializationVersion = 1;
	static constexpr std::uint32_t kContainers = 'EDCO';
	static constexpr std::uint32_t kNPCs = 'EDNP';

	void SaveCallback(SKSE::SerializationInterface* a_skse);
	void LoadCallback(SKSE::SerializationInterface* a_skse);
	void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse);
}