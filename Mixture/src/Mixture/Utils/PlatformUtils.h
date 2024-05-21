#pragma once

#include <string>

namespace Mixture {
	class FileDialogs {
	public:
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	};

	class Time {
	public:
		static float getTime();
	};
}
