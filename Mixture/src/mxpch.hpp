#pragma once

#include <string>
#include <functional>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mixture/Core/Base.hpp"

#ifdef MX_PLATFORM_WINDOWS
    #ifndef NOMINMAX
        // See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
        #define NOMINMAX
    #endif
#endif
