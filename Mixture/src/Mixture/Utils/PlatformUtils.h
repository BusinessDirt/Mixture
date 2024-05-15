#pragma once

#include <string>
#include <optional>

namespace Mixture {
	class FileDialogs {
	public:
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};
}
