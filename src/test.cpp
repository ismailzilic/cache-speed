#include "ll.h"
#include "timer.h"
#include "vm.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#if defined(_WIN32)
#include <windows.h>

#elif defined(__linux__)
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#endif

/*
 * Ryzen 5 5600 cache sizes (per-core):
 *
 * L1d cache: 32 KiB (6 instances)
 * L2 cache: 512 KiB (6 instances)
 * L3 cache: 32 MiB (1 instance)
 *
 */

#define KiB(n) ((n) << 10)
#define MiB(n) ((n) << 20)

constexpr size_t L1D_CAP = KiB(25) / sizeof(ll_node);
constexpr size_t L2_CAP = KiB(410) / sizeof(ll_node);
constexpr size_t L3_CAP = MiB(25) / sizeof(ll_node);

struct random_cycle {
	std::vector<ll_node *> nodes;
	void *region;
	size_t region_size;
};

static random_cycle make_random_cycle(size_t count)
{
	std::random_device rd;
	std::mt19937 rng{rd()};

	const size_t page_size = vm_get_page_size();
	size_t region_size = count * page_size;

#if defined(_WIN32)
	void *region = VirtualAlloc(NULL, region_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (region == NULL) {
		throw std::runtime_error("virtual alloc failed.\n");
	}

#elif defined(__linux__)
	void *region = mmap(nullptr, region_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (region == MAP_FAILED) {
		throw std::runtime_error("memory mapping failed.\n");
	}

	madvise(region, region_size, MADV_RANDOM);
#endif

	std::vector<size_t> page_offsets(count);
	std::iota(page_offsets.begin(), page_offsets.end(), 0);
	std::shuffle(page_offsets.begin(), page_offsets.end(), rng);

	std::vector<ll_node *> nodes(count);
	for (size_t i = 0; i < count; ++i) {
		nodes[i] = reinterpret_cast<ll_node *>(static_cast<char *>(region) + page_offsets[i] * page_size);
		nodes[i]->next = nullptr;
		nodes[i]->data = 0;
	}

	for (size_t i = 0; i < count; ++i) {
		nodes[i]->next = nodes[(i + 1) % count];
	}

	return {nodes, region, region_size};
}

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

static void run_test(size_t set_size, const char *label)
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

int main()
{
	run_test(L1D_CAP, "L1D");
	run_test(L2_CAP, "L2");
	run_test(L3_CAP, "L3");

	return 0;
}
