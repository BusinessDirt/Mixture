#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Mixture {
	struct ProfileResult {
		std::string name;
		long long start, end;
		uint32_t threadID;
	};

	struct InstrumentationSession {
		std::string name;
	};

	class Instrumentor {
	public:
		Instrumentor() : m_CurrentSession(nullptr), m_ProfileCount(0) {}

		void beginSession(const std::string& name, const std::string& filepath = "results.json") {
			m_OutputStream.open(filepath);
			writeHeader();
			m_CurrentSession = new InstrumentationSession{ name };
		}

		void endSession() {
			writeFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
			m_ProfileCount = 0;
		}

		void writeProfile(const ProfileResult& result) {
			if (m_ProfileCount++ > 0)
				m_OutputStream << ",";

			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.end - result.start) << ',';
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"tid\":" << result.threadID << ",";
			m_OutputStream << "\"ts\":" << result.start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void writeHeader() {
			m_OutputStream << "{\"otherData\": {}, \"traceEvents\":[";
			m_OutputStream.flush();
		}

		void writeFooter() {
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		static Instrumentor& get() {
			static Instrumentor instance;
			return instance;
		}
	private:
		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		int m_ProfileCount;
	};

	class InstrumentationTimer {
	public:
		InstrumentationTimer(const char* name) : m_Name(name), m_Stopped(false) {
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer() {
			if (!m_Stopped) stop();
		}

		void stop() {
			std::chrono::steady_clock::time_point endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::get().writeProfile({ m_Name, start, end, threadID });

			m_Stopped = true;
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}

#define MX_PROFILE 1
#if MX_PROFILE
	#define MX_PROFILE_BEGIN_SESSION(name, filepath) ::Mixture::Instrumentor::get().beginSession(name, filepath)
	#define MX_PROFILE_END_SESSION() ::Mixture::Instrumentor::get().endSession()
	#define MX_PROFILE_SCOPE(name) ::Mixture::InstrumentationTimer timer##__LINE__(name)
	#define MX_PROFILE_FUNCTION() MX_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define MX_PROFILE_BEGIN_SESSION(name, filepath)
	#define MX_PROFILE_END_SESSION()
	#define MX_PROFILE_SCOPE(name)
	#define MX_PROFILE_FUNCTION()
#endif