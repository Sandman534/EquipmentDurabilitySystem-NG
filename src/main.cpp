#include <stddef.h>
#include "DurabilityMenu.h"
#include "Events.h"
#include "Settings.h"
#include "Serialization.h"
#include "Utility.h"
#include "EDUI.h"
#include "Temper.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;


void SetupLog() {
	auto logsFolder = SKSE::log::log_directory();
	if (!logsFolder)
		SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
	auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
	auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
	auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
	auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
	spdlog::set_default_logger(std::move(loggerPtr));
	spdlog::set_level(spdlog::level::trace);
	spdlog::flush_on(spdlog::level::trace);
}

void InitListener(SKSE::MessagingInterface::Message* a_msg) {
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Utility::GetSingleton()->LoadForms();
		Settings::GetSingleton()->LoadINI();
		Events::Init();
		Menu::Init();
		TemperManager::Init();
		PlayerGraphEventHook::Install();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		SKSE::GetTaskInterface()->AddTask([]() { Menu::MenuInit(); });
		break;
	case SKSE::MessagingInterface::kNewGame:
		SKSE::GetTaskInterface()->AddTask([]() { Menu::MenuInit(); });
		break;
	}
}

SKSEPluginLoad(const LoadInterface* skse) {
	SetupLog();
	logger::info("{} {} is loading...", Plugin::NAME, HelperFunctions::VersionToString(Plugin::Version));
	Init(skse);
	SKSE::AllocTrampoline(128);

	// Message Listener
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener(InitListener)) {
		return false;
	}

	// Serialization to save/load information
	if (auto serialization = SKSE::GetSerializationInterface()) {
		serialization->SetUniqueID(Serialization::ID);
		serialization->SetSaveCallback(&Serialization::SaveCallback);
	}

	// Register the SKSE Menu
	EDUI::Register();
	logger::info("{} has finished loading.", Plugin::NAME);
	return true;
}