#include <stddef.h>
#include "DurabilityMenu.h"
#include "Events.h"
#include "FormLoader.h"
#include "Settings.h"
#include "Serialization.h"
#include "EDUI.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;


void SetupLog()
{
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

void InitListener(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		FormLoader::GetSingleton()->LoadAllForms();
		Settings::GetSingleton()->LoadINI();
		Events::Init();
		Menu::Init();
		Menu::AnimationInit();
		RE::UI::GetSingleton()->Register("DurabilityMenu", &DurabilityMenu::Create);
		break;
	}
}

SKSEPluginLoad(const LoadInterface* skse)
{
	SetupLog();
	logger::info("{} {} is loading...", Plugin::NAME, REL::Module::get().version().string());

	Init(skse);
	FormLoader::GetSingleton()->CacheGameAddresses();
	SKSE::AllocTrampoline(42);

	// Form Loader
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener(InitListener)) {
		return false;
	}

	// Load Dynamic Processing Cache
	if (auto serialization = SKSE::GetSerializationInterface()) {
		serialization->SetUniqueID(Serialization::ID);
		serialization->SetSaveCallback(&Serialization::SaveCallback);
		serialization->SetLoadCallback(&Serialization::LoadCallback);
		serialization->SetRevertCallback(&Serialization::RevertCallback);
	}

	EDUI::Register();
	logger::info("{} has finished loading.", Plugin::NAME);
	return true;
}