#include "Serialization.h"
#include "Settings.h"
#include "Utility.h"
#include "Events.h"
#include "DurabilityMenu.h"

namespace Serialization {
	void SaveCallback(SKSE::SerializationInterface* a_skse) {
		// Save the settings to file
		auto* setting = Settings::GetSingleton();
		setting->SaveINI();
		
		// Cache serialization lambda
		auto writeSet = [&](std::uint32_t type, const std::unordered_set<RE::TESObjectREFR*>& set) {
			if (!a_skse->OpenRecord(type, kVersion)) return;

			std::uint32_t size = static_cast<std::uint32_t>(set.size());
			a_skse->WriteRecordData(&size, sizeof(size));

			for (RE::TESObjectREFR* ref : set) {
				RE::ObjectRefHandle handle{};
				if (ref) handle = ref->CreateRefHandle();
				uint32_t raw = handle.native_handle();
				a_skse->WriteRecordData(&raw, sizeof(raw));
			}
		};

		// Write the cache to the file
		writeSet(kContainers, setting->processedContainers);
		writeSet(kNPCs, setting->processedNPCs);

		// Write HUD state
        if (!a_skse->OpenRecord(kState, kVersion)) return;
        std::uint8_t packed = 0;
        if (DurabilityMenu::GetSingleton()->sheathActivated) packed |= 0x01;
        if (DurabilityMenu::GetSingleton()->hotkeyActivated) packed |= 0x02;
		a_skse->WriteRecordData(&packed, sizeof(packed));

	}

	void LoadCallback(SKSE::SerializationInterface* a_skse) {
		std::uint32_t type, version, length;

		// Clear the caches
		auto* setting = Settings::GetSingleton();
		setting->processedContainers.clear();
		setting->processedNPCs.clear();

		// Insert the saved cache information
		while (a_skse->GetNextRecordInfo(type, version, length)) {
			std::uint32_t size;
			a_skse->ReadRecordData(&size, sizeof(size));

			// Cache reading lambda
			auto readSet = [&](std::unordered_set<RE::TESObjectREFR*>& set) {
				for (std::uint32_t i = 0; i < size; i++) {
					std::uint32_t count = 0;
					a_skse->ReadRecordData(&count, sizeof(count));

					for (uint32_t i = 0; i < count; i++) {
						std::uint32_t raw = 0;
						a_skse->ReadRecordData(&raw, sizeof(raw));

						RE::NiPointer<RE::TESObjectREFR> objectRefHandle;
						RE::TESObjectREFR* objectRef;

						if (RE::LookupReferenceByHandle(raw, objectRefHandle)) {
							objectRef = objectRefHandle.get();
							if (objectRef) set.insert(objectRef);					
						}
					}
				}
			};
			
			// Process the incoming data types
			if (type == kContainers && version == kVersion) readSet(setting->processedContainers);
			else if (type == kNPCs && version == kVersion) readSet(setting->processedNPCs);

			// Restore HUD state
			else if (type == kState && version == kVersion) {
				std::uint8_t packed = 0;
				a_skse->ReadRecordData(&packed, sizeof(packed));
				DurabilityMenu::GetSingleton()->sheathActivated = (packed & 0x01) != 0;
				DurabilityMenu::GetSingleton()->hotkeyActivated = (packed & 0x02) != 0;
				DurabilityMenu::GetSingleton()->ShowMenu();
			}
		}
	}

	void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse) {
		// Reset the caches for loading
		auto* setting = Settings::GetSingleton();
		setting->processedContainers.clear();
		setting->processedNPCs.clear();

		// Reset HUD state
		DurabilityMenu::GetSingleton()->sheathActivated = false;
		DurabilityMenu::GetSingleton()->hotkeyActivated = false;

	}
}