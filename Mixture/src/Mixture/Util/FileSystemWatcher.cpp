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
                            m_FilePaths[entry.path().string()] = { entry.last_write_time(), m_ScanGeneration };
                        }
                    }
                }
                else
                {
                    for (auto& entry : std::filesystem::directory_iterator(m_Path))
                    {
                        if (!entry.is_directory())
                        {
                            m_FilePaths[entry.path().string()] = { entry.last_write_time(), m_ScanGeneration };
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

            ScanOnce();
        }
    }

    void FileSystemWatcher::ScanOnce()
    {
        if (!std::filesystem::exists(m_Path)) return;
        ++m_ScanGeneration;

        try
        {
            auto processEntry = [&](const std::filesystem::directory_entry& entry)
            {
                if (entry.is_directory()) return;
                const std::string pathString = entry.path().string();
                const auto lastWriteTime = entry.last_write_time();
                auto [it, inserted] = m_FilePaths.try_emplace(pathString,
                    TrackedFile{ lastWriteTime, m_ScanGeneration });
                if (inserted)
                {
                    if (m_Callback) m_Callback(entry.path(), FileAction::Added);
                }
                else
                {
                    if (it->second.LastWriteTime != lastWriteTime)
                    {
                        it->second.LastWriteTime = lastWriteTime;
                        if (m_Callback) m_Callback(entry.path(), FileAction::Modified);
                    }
                    it->second.SeenGeneration = m_ScanGeneration;
                }
            };

            if (m_Recursive)
            {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(m_Path)) processEntry(entry);
            }
            else
            {
                for (const auto& entry : std::filesystem::directory_iterator(m_Path)) processEntry(entry);
            }

            for (auto it = m_FilePaths.begin(); it != m_FilePaths.end(); )
            {
                if (it->second.SeenGeneration == m_ScanGeneration)
                {
                    ++it;
                    continue;
                }
                const std::filesystem::path deletedPath(it->first);
                it = m_FilePaths.erase(it);
                if (m_Callback) m_Callback(deletedPath, FileAction::Deleted);
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            OPAL_WARN("Core", "FileSystemWatcher error: {}", e.what());
        }
    }
}
