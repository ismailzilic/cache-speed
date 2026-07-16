#include "benchmark.h"
#include "hw.h"
#include "ll.h"
#include "timer.h"
#include "user-flags.h"
#include "util.h"

#include <iostream>

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

static void run_test(const char *label, size_t set_size, uint16_t flags)
{
	random_cycle cycle;
	size_t nodes{};

	constexpr size_t cache_line_size = 64;
	nodes = static_cast<size_t>((set_size * 1024 * 0.8) / cache_line_size);

	cycle = make_random_cycle(nodes, flags);

	double read_ns = make_measurement(cycle, nodes, [](ll_node *, size_t) {});
	double write_ns = make_measurement(cycle, nodes, [](ll_node *p, size_t i) { p->data = i; });

	std::cout << label << " read time: " << read_ns << "ns | " << read_ns / 1000.0 << "us" << std::endl;
	std::cout << label << " write time: " << write_ns << "ns | " << write_ns / 1000.0 << "us" << std::endl;

	destroy_random_cycle(cycle);
}

void delegate_tests(const uint16_t flags)
{
	if (flags & HELP) {
		print_help();
		return;
	}

	cache_size sizes = poll_cache_sizes();

	run_test("L1D", sizes.L1, flags);
	run_test("L2", sizes.L2, flags);
	run_test("L3", sizes.L3, flags);
}
