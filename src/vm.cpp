#include "vm.h"

#if defined(_WIN32)

#include <windows.h>

#elif defined(__linux__)

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <unistd.h>

#else

#include <stdexcept>

#endif

uint32_t get_page_size(void)
{
#if defined(_WIN32)
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwPageSize;
#elif defined(__linux__)
	return static_cast<uint32_t>(sysconf(_SC_PAGESIZE));
#else
	throw std::runtime_error("unsupported platform.\n");
#endif
}
