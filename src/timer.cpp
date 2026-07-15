#include "timer.h"

namespace Measurements
{

void Timer::TimeSnapshot()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

double Timer::GetNanoseconds()
{
	m_EndTime = std::chrono::high_resolution_clock::now();

	double startTimepoint = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTime).time_since_epoch().count();
	double endTimepoint = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_EndTime).time_since_epoch().count();

	return endTimepoint - startTimepoint;
}

} // namespace Measurements
