#include "benchmark.h"
#include "ll.h"
#include "timer.h"
#include "util.h"

#include <cstdint>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sys/mman.h>
#endif

template <typename Fn> static double measure_cache(ll_node *p, const size_t repeats, const size_t set_size, Fn per_node)
{
	Measurements::Timer timer;
	timer.TimeSnapshot();

	for (size_t r = 0; r < repeats; r++) {
		for (size_t i = 0; i < set_size; i++) {
			per_node(p, i);
			p = p->next;
		}
	}

	double ret = timer.GetNanoseconds() / (double)(repeats * set_size);
	volatile uintptr_t sink = (uintptr_t)p;

	return ret;
}

template <typename Fn> static double make_measurement(const random_cycle &cycle, const size_t set_size, Fn per_node)
{
	ll_node *p = cycle.nodes[0];
	size_t repeats{max_size_t(10, (10'000'000 / set_size))};

	double probing_time = measure_cache(p, repeats, set_size, per_node);

	if (probing_time < 1.0) {
		repeats *= 2;
		probing_time = measure_cache(p, repeats, set_size, per_node);
	}

	constexpr double target_ns = 1.5e9;
	size_t repeat_test = target_ns / (probing_time * set_size);

	return measure_cache(p, repeat_test, set_size, per_node);
}

void run_test(size_t set_size, const char *label)
{
	constexpr size_t cache_line_size = 64;
	size_t nodes = static_cast<size_t>((set_size * 1024 * 0.8) / cache_line_size);
	random_cycle cycle = make_random_cycle(nodes);

	double read_ns = make_measurement(cycle, nodes, [](ll_node *, size_t) {});
	double write_ns = make_measurement(cycle, nodes, [](ll_node *p, size_t i) { p->data = i; });

	std::cout << label << " read time: " << read_ns << "ns | " << read_ns / 1000.0 << "us" << std::endl;
	std::cout << label << " write time: " << write_ns << "ns | " << write_ns / 1000.0 << "us" << std::endl;

#if defined(_WIN32)
	VirtualFree(cycle.region, 0, MEM_RELEASE);
#elif defined(__linux__)
	munmap(cycle.region, cycle.region_size);
#endif
}
