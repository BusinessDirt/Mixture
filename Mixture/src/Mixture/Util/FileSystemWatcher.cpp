#include "mxpch.hpp"
#include "Mixture/Util/FileSystemWatcher.hpp"

namespace Mixture
{
    FileSystemWatcher::FileSystemWatcher(const std::filesystem::path& path, Callback callback, bool recursive)
        : m_Path(path), m_Callback(std::move(callback)), m_Recursive(recursive), m_Running(false)
    {
        // Initial scan to populate the map so we don't trigger "Added" events for existing files on startup
        if (std::filesystem::exists(m_Path))
        {
            try
            {
                if (m_Recursive)
                {
                    for (auto& entry : std::filesystem::recursive_directory_iterator(m_Path))
                    {
                        if (!entry.is_directory())
                        {
                            m_FilePaths[entry.path().string()] = entry.last_write_time();
                        }
                    }
                }
                else
                {
                    for (auto& entry : std::filesystem::directory_iterator(m_Path))
                    {
                        if (!entry.is_directory())
                        {
                            m_FilePaths[entry.path().string()] = entry.last_write_time();
                        }
                    }
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                OPAL_ERROR("Core", "FileSystemWatcher initial scan failed: {}", e.what());
            }
        }
    }

    FileSystemWatcher::~FileSystemWatcher()
    {
        Stop();
    }

    void FileSystemWatcher::Start()
    {
        if (m_Running)
            return;

        m_Running = true;
        m_Thread = std::thread(&FileSystemWatcher::WatchLoop, this);
    }

    void FileSystemWatcher::Stop()
    {
        if (!m_Running) return;

        m_Running = false;
        if (m_Thread.joinable()) m_Thread.join();
    }

    void FileSystemWatcher::WatchLoop()
    {
        Opal::LogRegistry::SetThreadName("Watcher");
        while (m_Running)
        {
            // Wait for a bit to avoid high CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            if (!std::filesystem::exists(m_Path))
                continue;

            auto currentFiles = m_FilePaths; // Copy current state to mark deletions
            std::unordered_map<std::string, std::filesystem::file_time_type> newFiles;

            try
            {
                // Helper lambda to process an entry
                auto processEntry = [&](const std::filesystem::directory_entry& entry)
                {
                    if (entry.is_directory()) return;

                    std::string pathStr = entry.path().string();
                    auto lastWriteTime = entry.last_write_time();

                    newFiles[pathStr] = lastWriteTime;

                    // Check if new or modified
                    auto it = m_FilePaths.find(pathStr);
                    if (it == m_FilePaths.end())
                    {
                        // Added
                        m_FilePaths[pathStr] = lastWriteTime;
                        if (m_Callback) m_Callback(entry.path(), FileAction::Added);
                    }
                    else
                    {
                        // Exists, check modified
                        if (it->second != lastWriteTime)
                        {
                            m_FilePaths[pathStr] = lastWriteTime;
                            if (m_Callback) m_Callback(entry.path(), FileAction::Modified);
                        }
                        // Remove from the 'currentFiles' copy to track deletions
                        currentFiles.erase(pathStr);
                    }
                };

                if (m_Recursive)
                {
                    for (auto& entry : std::filesystem::recursive_directory_iterator(m_Path))
                        processEntry(entry);
                }
                else
                {
                    for (auto& entry : std::filesystem::directory_iterator(m_Path))
                        processEntry(entry);
                }

                // Any files left in 'currentFiles' were not found in the new scan -> Deleted
                for (const auto& [path, time] : currentFiles)
                {
                    m_FilePaths.erase(path);
                    if (m_Callback) m_Callback(std::filesystem::path(path), FileAction::Deleted);
                }

            }
            catch (const std::filesystem::filesystem_error& e)
            {
                // Can happen if permission denied or directory deleted mid-scan
                OPAL_WARN("Core", "FileSystemWatcher error: {}", e.what());
            }
        }
    }
}
