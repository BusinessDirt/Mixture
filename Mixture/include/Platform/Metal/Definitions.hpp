#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Definitions.hpp
 * @brief Common definitions and structures for Metal.
 */

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#if defined(__OBJC__)
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

#include "Mixture/Core/Base.hpp"

#endif
