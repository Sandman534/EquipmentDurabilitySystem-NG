#include "Serialization.h"
#include "Settings.h"
#include "Utility.h"
#include "Events.h"
#include "DurabilityMenu.h"

namespace Serialization {
	void SaveCallback(SKSE::SerializationInterface* a_skse) {
		Settings::GetSingleton()->SaveINI();
	}
}