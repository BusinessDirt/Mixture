#pragma once

#include "Mixture/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Mixture {
	class Log {
	public:
		static void Init();
		
		static Ref<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& getClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion) {
	return os << glm::to_string(quaternion);
}

#ifndef MIXTURE_DIST

// Core log macros
#define MX_CORE_TRACE(...)     ::Mixture::Log::getCoreLogger()->trace(__VA_ARGS__)
#define MX_CORE_INFO(...)      ::Mixture::Log::getCoreLogger()->info(__VA_ARGS__)
#define MX_CORE_WARN(...)      ::Mixture::Log::getCoreLogger()->warn(__VA_ARGS__)
#define MX_CORE_ERROR(...)     ::Mixture::Log::getCoreLogger()->error(__VA_ARGS__)
#define MX_CORE_CRITICAL(...)     ::Mixture::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MX_TRACE(...)   ::Mixture::Log::getClientLogger()->trace(__VA_ARGS__)
#define MX_INFO(...)    ::Mixture::Log::getClientLogger()->info(__VA_ARGS__)
#define MX_WARN(...)    ::Mixture::Log::getClientLogger()->warn(__VA_ARGS__)
#define MX_ERROR(...)   ::Mixture::Log::getClientLogger()->error(__VA_ARGS__)
#define MX_CRITICAL(...)   ::Mixture::Log::getClientLogger()->critical(__VA_ARGS__)

#else // strip logging in dist build

// Core log macros
#define MX_CORE_TRACE(...)
#define MX_CORE_INFO(...)
#define MX_CORE_WARN(...)
#define MX_CORE_ERROR(...)
#define MX_CORE_FATAL(...)

// Client log macros
#define MX_TRACE(...)
#define MX_INFO(...)
#define MX_WARN(...)
#define MX_ERROR(...)
#define MX_FATAL(...)

#endif
