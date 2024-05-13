#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "Mixture/Logging/Log.h"

namespace Mixture {

	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult {
		std::string name;
		FloatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession {
		std::string name;
	};

	class Instrumentor {
	public:
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;

		void beginSession(const std::string& name, const std::string& filepath = "results.json") {
			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession) {
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Log::getCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
					MX_CORE_ERROR("Instrumentor::beginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->name);
				}
				internalEndSession();
			}
			m_OutputStream.open(filepath);
			if (m_OutputStream.is_open()) {
				m_CurrentSession = new InstrumentationSession({ name });
				writeHeader();
			} else {
				if (Log::getCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
					MX_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
				}
			}
		}

		void endSession() {
			std::lock_guard lock(m_Mutex);
			internalEndSession();
		}

		void writeProfile(const ProfileResult& result) {
			std::stringstream json;

			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.elapsedTime.count()) << ',';
			json << "\"name\":\"" << result.name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.threadID << ",";
			json << "\"ts\":" << result.start.count();
			json << "}";

			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession) {
				m_OutputStream << json.str();
				m_OutputStream.flush();
			}
		}

		static Instrumentor& get() {
			static Instrumentor instance;
			return instance;
		}
	private:
		Instrumentor() : m_CurrentSession(nullptr) {}
		~Instrumentor() { endSession(); }

		void writeHeader() {
			m_OutputStream << "{\"otherData\": {}, \"traceEvents\":[{}";
			m_OutputStream.flush();
		}

		void writeFooter() {
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		// Note: you must already own lock on m_Mutex before
		// calling InternalEndSession()
		void internalEndSession() {
			if (m_CurrentSession) {
				writeFooter();
				m_OutputStream.close();
				delete m_CurrentSession;
				m_CurrentSession = nullptr;
			}
		}
	private:
		std::mutex m_Mutex;
		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
	};

	class InstrumentationTimer {
	public:
		InstrumentationTimer(const char* name) : m_Name(name), m_Stopped(false) {
			m_StartTimepoint = std::chrono::steady_clock::now();
		}

		~InstrumentationTimer() {
			if (!m_Stopped) stop();
		}

		void stop() {
			std::chrono::steady_clock::time_point endTimepoint = std::chrono::steady_clock::now();
			FloatingPointMicroseconds highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
			std::chrono::microseconds elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

			Instrumentor::get().writeProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

			m_Stopped = true;
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};

	namespace InstrumentorUtils {
		template<size_t N>
		struct ChangeResult {
			char data[N];
		};

		template<size_t N, size_t K>
		constexpr ChangeResult<N> cleanupOutputString(const char(&expr)[N], const char(&remove)[K]) {
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N) {
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}
}

#define MX_PROFILE 0
#if MX_PROFILE
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define MX_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define MX_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define MX_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define MX_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define MX_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define MX_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define MX_FUNC_SIG __func__
	#else
		#define MX_FUNC_SIG "MX_FUNC_SIG unknown!"
	#endif

	#define MX_PROFILE_BEGIN_SESSION(name, filepath) ::Mixture::Instrumentor::get().beginSession(name, filepath)
	#define MX_PROFILE_END_SESSION() ::Mixture::Instrumentor::get().endSession()
	#define MX_PROFILE_SCOPE_LINE(name, line) constexpr auto fixedName##line = ::Mixture::InstrumentorUtils::cleanupOutputString(name, "__cdecl");\
											::Mixture::InstrumentationTimer timer##line(fixedName##line.Data)
	#define MX_PROFILE_SCOPE(name) MX_PROFILE_SCOPE_LINE(name, __LINE__)
	#define MX_PROFILE_FUNCTION() MX_PROFILE_SCOPE(MX_FUNC_SIG)
#else
	#define MX_PROFILE_BEGIN_SESSION(name, filepath)
	#define MX_PROFILE_END_SESSION()
	#define MX_PROFILE_SCOPE(name)
	#define MX_PROFILE_FUNCTION()
#endif