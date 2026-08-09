#pragma once

#include "Opal/Base.hpp"
#include "Opal/Formatters/GLMFormatters.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

#include <atomic>
#include <shared_mutex>

namespace Opal
{
    /**
     * @brief Builder class for configuring and creating spdlog sinks.
     */
    class LogBuilder
    {
    public:
        LogBuilder() = default;

        /**
         * @brief Adds a console sink to the builder.
         *
         * @param level The log level for this sink (default: trace).
         * @return LogBuilder& Reference to this builder for chaining.
         */
        LogBuilder& UseConsoleSink(spdlog::level::level_enum level = spdlog::level::trace);

        /**
         * @brief Adds a file sink to the builder.
         *
         * @param filepath The path to the log file.
         * @param level The log level for this sink (default: trace).
         * @return LogBuilder& Reference to this builder for chaining.
         */
        LogBuilder& UseFileSink(const std::string& filepath, spdlog::level::level_enum level = spdlog::level::trace);

        /**
         * @brief Adds a custom sink to the builder.
         *
         * @param sink The shared pointer to the custom sink.
         * @return LogBuilder& Reference to this builder for chaining.
         */
        LogBuilder& AddSink(std::shared_ptr<spdlog::sinks::sink> sink)
        {
            m_Sinks.push_back(sink);
            return *this;
        }

        /**
         * @brief Builds and returns the vector of configured sinks.
         *
         * @return std::vector<spdlog::sink_ptr> The list of sinks.
         */
        std::vector<spdlog::sink_ptr> Build() const { return m_Sinks; }

    private:
        std::vector<spdlog::sink_ptr> m_Sinks;
    };

    /**
     * @brief Singleton registry for managing loggers and their sinks.
     */
    class LogRegistry
    {
    public:
        /**
         * @brief Gets the singleton instance of the LogRegistry.
         *
         * @return LogRegistry& Reference to the singleton instance.
         */
        static LogRegistry& Get()
        {
            static LogRegistry instance;
            return instance;
        }

        /**
         * @brief Initializes the registry with a set of sinks.
         *
         * These sinks will be used for all loggers created by the registry.
         *
         * @param sinks The vector of sinks to use.
         */
        void Initialize(const std::vector<spdlog::sink_ptr>& sinks);

        /**
         * @brief Retrieves or creates a logger with the specified name.
         *
         * If the logger already exists, it is returned. Otherwise, a new logger
         * is created using the registered sinks.
         *
         * @param name The name of the logger.
         * @return std::shared_ptr<spdlog::logger> The logger instance.
         */
        std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);

        /** Flushes every cached logger. Normal messages flush only on errors. */
        void FlushAll();

        uint64_t GetRegistryLookupCount() const { return m_RegistryLookupCount.load(); }

        /**
         * @brief Sets the name of the current thread for logging purposes.
         * 
         * @param name The name to assign to the current thread.
         */
        static void SetThreadName(const std::string& name);

        /**
         * @brief Gets the name of the current thread.
         * 
         * @return const std::string& The thread name.
         */
        static const std::string& GetThreadName();

    private:
        LogRegistry() = default; // Singleton
        std::vector<spdlog::sink_ptr> m_Sinks;
        std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_Loggers;
        mutable std::shared_mutex m_Mutex;
        std::atomic<uint64_t> m_RegistryLookupCount{ 0 };
    };
}

#ifndef OPAL_DIST

    /**
     * @brief Logs a trace message.
     */
    #define OPAL_TRACE(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->trace(__VA_ARGS__)

    #ifdef OPAL_DEBUG

        /**
         * @brief Logs a debug message.
         */
        #define OPAL_LOG_DEBUG(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->debug(__VA_ARGS__)

    #else

        #define OPAL_LOG_DEBUG(...)

    #endif

    /**
     * @brief Logs an info message.
     */
    #define OPAL_INFO(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->info(__VA_ARGS__)

    /**
     * @brief Logs a warning message.
     */
    #define OPAL_WARN(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->warn(__VA_ARGS__)

    /**
     * @brief Logs an error message.
     */
    #define OPAL_ERROR(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->error(__VA_ARGS__)

    /**
     * @brief Logs a critical message.
     */
    #define OPAL_CRITICAL(logger, ...) ::Opal::LogRegistry::Get().GetLogger(logger)->critical(__VA_ARGS__)

#else // strip logging in dist build
    #define OPAL_TRACE(...)
    #define OPAL_LOG_DEBUG(...)
    #define OPAL_INFO(...)
    #define OPAL_WARN(...)
    #define OPAL_ERROR(...)
    #define OPAL_CRITICAL(...)
#endif
