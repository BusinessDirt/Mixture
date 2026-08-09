#pragma once

/**
 * @file FileSystemWatcher.hpp
 * @brief Utility class for monitoring file system changes.
 */

#include "Mixture/Core/Base.hpp"

#include <filesystem>
#include <functional>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <string>

namespace Mixture
{
    /**
     * @brief Enum representing the type of file system action detected.
     */
    enum class FileAction
    {
        Added,
        Modified,
        Deleted
    };

    /**
     * @brief A utility class that watches a directory for changes to files.
     * 
     * Uses a background thread to poll for file system changes.
     * Useful for hot-reloading assets like shaders or scripts.
     */
    class FileSystemWatcher
    {
    public:
        /**
         * @brief Callback function type for file events.
         * 
         * @param path The path of the file that changed.
         * @param action The type of change that occurred.
         */
        using Callback = std::function<void(const std::filesystem::path& path, FileAction action)>;

        /**
         * @brief Constructs a FileSystemWatcher.
         * 
         * @param path The directory path to watch.
         * @param callback The callback function to execute on change.
         * @param recursive Whether to scan subdirectories recursively. Default is true.
         */
        FileSystemWatcher(const std::filesystem::path& path, Callback callback, bool recursive = true);

        /**
         * @brief Destructor. Stops the watcher if it is running.
         */
        ~FileSystemWatcher();

        /**
         * @brief Starts the background watching thread.
         */
        void Start();

        /**
         * @brief Stops the background watching thread.
         */
        void Stop();

        /**
         * @brief Checks if the watcher is currently running.
         * 
         * @return true If running.
         */
        bool IsRunning() const { return m_Running; }

        /** @brief Performs one polling scan. Exposed for deterministic tests and instrumentation. */
        void ScanOnce();

        size_t GetTrackedFileCount() const { return m_FilePaths.size(); }

    private:
        void WatchLoop();

    private:
        std::filesystem::path m_Path;
        Callback m_Callback;
        bool m_Recursive;

        std::atomic<bool> m_Running;
        std::thread m_Thread;

        // Map of filepath string to last write time
        struct TrackedFile
        {
            std::filesystem::file_time_type LastWriteTime;
            uint64_t SeenGeneration = 0;
        };
        std::unordered_map<std::string, TrackedFile> m_FilePaths;
        uint64_t m_ScanGeneration = 0;
    };
}
