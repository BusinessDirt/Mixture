#pragma once

#include <string>

namespace Mixture {
	class FileDialogs {
	public:
		// these return empty strings if cancelled
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};
}
