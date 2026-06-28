#include <stddef.h>
#include "DurabilityMenu.h"
#include "Events.h"
#include "Settings.h"
#include "Utility.h"
#include "EDUI.h"
#include "Temper.h"

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
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

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	REL::Module::reset();
	 
	// Message Listener
	auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));

	if (!g_messaging) {
		logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}
	 
	logger::info("{} v{} is loading..."sv, Plugin::NAME, Plugin::VERSION.string());
	 
	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(256);

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	// Register the SKSE Menu
	EDUI::Register();
	logger::info("{} has finished loading.", Plugin::NAME);
	return true;
}