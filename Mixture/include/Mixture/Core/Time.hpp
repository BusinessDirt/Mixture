#pragma once
#include <chrono>

namespace Mixture
{
    class Timer
    {
    public:
        Timer() { Reset(); }

        void Reset()
        {
            m_Start = std::chrono::high_resolution_clock::now();
        }

        OPAL_NODISCARD float Elapsed() const
        {
            const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start);
            return static_cast<float>(time.count()) * 0.001f * 0.001f * 0.001f;
        }

        OPAL_NODISCARD float ElapsedMillis() const
        {
            return Elapsed() * 1000.0f;
        }
        
        OPAL_NODISCARD float Tick()
        {
            const float elapsed = Elapsed();
            Reset();
            return elapsed;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };
}


