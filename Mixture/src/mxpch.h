#pragma once

#include "Mixture/Core/PlatformDetection.h"

#ifdef MX_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Mixture/Core/Base.h"
#include "Mixture/Logging/Log.h"
#include "Mixture/Debug/Instrumentor.h"

#ifdef MIXTURE_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
