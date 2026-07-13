#pragma once

#include <chrono>

namespace Measurements
{
class Timer
{
      private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime, m_EndTime;

      public:
	void TimeSnapshot();
	double GetMicroseconds();
	double GetMiliseconds();
	double GetNanoseconds();
};
} // namespace Measurements
