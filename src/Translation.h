#pragma once

#include <string>

namespace EDTranslation {
	void Install();
	const char* Translate(const std::string& key);
}
