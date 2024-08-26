#pragma once

#include "Mixture/Core/Base.hpp"

#include <chrono>

namespace Mixture
{
	class Timestep
	{
	public:
		Timestep()
			: m_LastTime(std::chrono::high_resolution_clock::now()), m_Timestep(0.0f)
		{}

		operator float() const { return m_Timestep; }

		void OnUpdate()
		{
			auto newTime = std::chrono::high_resolution_clock::now();
			m_Timestep = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - m_LastTime).count();
			m_LastTime = newTime;
		}

	private:
		std::chrono::steady_clock::time_point m_LastTime;
		float m_Timestep;
	};
}