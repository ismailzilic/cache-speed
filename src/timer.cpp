#include "timer.h"

#include <chrono>

namespace Measurements
{

void Timer::TimeSnapshot()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

double Timer::GetMicroseconds()
{
	m_EndTime = std::chrono::high_resolution_clock::now();

	double startTimepoint = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
	double endTimepoint = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTime).time_since_epoch().count();

	return endTimepoint - startTimepoint;
}

double Timer::GetMiliseconds()
{
	m_EndTime = std::chrono::high_resolution_clock::now();

	double startTimepoint = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTime).time_since_epoch().count();
	double endTimepoint = std::chrono::time_point_cast<std::chrono::milliseconds>(m_EndTime).time_since_epoch().count();

	return endTimepoint - startTimepoint;
}

double Timer::GetNanoseconds()
{
	m_EndTime = std::chrono::high_resolution_clock::now();

	double startTimepoint = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTime).time_since_epoch().count();
	double endTimepoint = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_EndTime).time_since_epoch().count();

	return endTimepoint - startTimepoint;
}

} // namespace Measurements
