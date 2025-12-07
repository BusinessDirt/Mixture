#pragma once
#include <chrono>

namespace Mixture
{
    /**
     * @brief A simple high-resolution timer.
     */
    class Timer
    {
    public:
        Timer() { Reset(); }

        /**
         * @brief Resets the timer to the current time.
         */
        void Reset()
        {
            m_Start = std::chrono::high_resolution_clock::now();
        }

        /**
         * @brief Gets the elapsed time in seconds.
         * 
         * @return float Elapsed seconds.
         */
        OPAL_NODISCARD float Elapsed() const
        {
            const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start);
            return static_cast<float>(time.count()) * 0.001f * 0.001f * 0.001f;
        }

        /**
         * @brief Gets the elapsed time in milliseconds.
         * 
         * @return float Elapsed milliseconds.
         */
        OPAL_NODISCARD float ElapsedMillis() const
        {
            return Elapsed() * 1000.0f;
        }
        
        /**
         * @brief Returns the elapsed time since last reset, and resets the timer.
         * 
         * @return float Elapsed seconds since last reset.
         */
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


