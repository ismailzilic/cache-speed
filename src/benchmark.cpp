#include "benchmark.h"
#include "cache-info.h"
#include "timer.h"

#include <cassert>
#include <cstdint>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>

#elif defined(__linux__)
#include <sys/mman.h>

#endif

static size_t calc_repeats(const size_t set_size)
{
	// !!! SHOULD CALCULATE REPEAT SIZES AT RUNTIME !!!
	// Return values here will be set as some arbitrary nubmers

	if (set_size == L1D_CAP) {
		return 62500;
	}

	if (set_size == L2_CAP)
		return 2250;

	return 5; // For L3
}

template <typename Fn> static double time_cycle(const random_cycle &cycle, size_t set_size, Fn per_node)
{
	Measurements::Timer timer;

	size_t repeats{calc_repeats(set_size)};

	ll_node *p = cycle.nodes[0];
	timer.TimeSnapshot();

	for (size_t r = 0; r < repeats; r++) {
		for (size_t i = 0; i < set_size; i++) {
			per_node(p, i);
			p = p->next;
		}
	}

	volatile uintptr_t sink = (uintptr_t)p;
	return timer.GetNanoseconds() / (double)(repeats * set_size);
}

void run_test(size_t set_size, const char *label)
{
	assert((set_size == L1D_CAP || set_size == L2_CAP || set_size == L3_CAP) && "invalid set size.");

	random_cycle cycle = make_random_cycle(set_size);

	double read_ns = time_cycle(cycle, set_size, [](ll_node *, size_t) {});
	double write_ns = time_cycle(cycle, set_size, [](ll_node *p, size_t i) { p->data = i; });

	std::cout << label << " read time: " << read_ns << "ns | " << read_ns / 1000.0 << "us" << std::endl;
	std::cout << label << " write time: " << write_ns << "ns | " << write_ns / 1000.0 << "us" << std::endl;

#if defined(_WIN32)
	VirtualFree(cycle.region, 0, MEM_RELEASE);
#elif defined(__linux__)
	munmap(cycle.region, cycle.region_size);
#endif
}
