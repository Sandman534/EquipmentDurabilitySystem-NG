#include "Serialization.h"
#include "Settings.h"
#include "Utility.h"
#include "Events.h"

namespace Serialization {
	void SaveCallback(SKSE::SerializationInterface* a_skse) {

		auto* setting = Settings::GetSingleton();

		// Save the settings to file
		setting->SaveINI();
		
		// Serialization lambda
		auto writeSet = [&](std::uint32_t type, const std::unordered_set<RE::TESObjectREFR*>& set) {
			if (!a_skse->OpenRecord(type, 1)) return;

			std::uint32_t size = static_cast<std::uint32_t>(set.size());
			a_skse->WriteRecordData(&size, sizeof(size));

			for (RE::TESObjectREFR* ref : set) {
				RE::ObjectRefHandle handle{};
				if (ref) handle = ref->CreateRefHandle();
				uint32_t raw = handle.native_handle();
				a_skse->WriteRecordData(&raw, sizeof(raw));
			}
		};

		// Write the records to file
		writeSet(kContainers, setting->processedContainers);
		writeSet(kNPCs, setting->processedNPCs);

		// How many records did we log
		logger::info("Saved {} records into Containers Cache", setting->processedContainers.size());
		logger::info("Saved {} records into NPC Cache", setting->processedNPCs.size());
	}

	void LoadCallback(SKSE::SerializationInterface* a_skse) {
		std::uint32_t type, version, length;

		auto* setting = Settings::GetSingleton();

		// Clear the caches
		setting->processedEquipment.clear();
		setting->processedContainers.clear();
		setting->processedNPCs.clear();

		// Insert the saved cache information
		while (a_skse->GetNextRecordInfo(type, version, length)) {
			std::uint32_t size;
			a_skse->ReadRecordData(&size, sizeof(size));

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
			
			// Process the incoming types
			if (type == kContainers) readSet(setting->processedContainers);
			else if (type == kNPCs) readSet(setting->processedNPCs);
		}

		// Log the records loaded
		logger::info("Loaded {} records into Equipment Cache", setting->processedEquipment.size());
		logger::info("Loaded {} records into Containers Cache", setting->processedContainers.size());
		logger::info("Loaded {} records into NPC Cache", setting->processedNPCs.size());
	}

	void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse) {
		auto* setting = Settings::GetSingleton();
		setting->processedEquipment.clear();
		setting->processedContainers.clear();
		setting->processedNPCs.clear();
		logger::debug("Cleared All Caches");
	}
}