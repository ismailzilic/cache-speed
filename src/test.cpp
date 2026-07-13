#include "ll.h"
#include "vm.h"
#include "timer.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

/*
 * i5-7300U cache sizes:
 *
 * L1d cache: 64 KiB (2 instances)
 * L2 cache: 512 KiB (2 instances)
 * L3 cache: 3 MiB (1 instance)
 *
 */

#define KiB(n) (n << 10)
#define MiB(n) (n << 20)

constexpr size_t L1D_LIM = KiB(64) / sizeof(ll_node);
constexpr size_t L2_LIM = KiB(512) / sizeof(ll_node);
constexpr size_t L3_LIM = MiB(3) / sizeof(ll_node);

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
	size_t overallocate = count * 8;
	size_t region_size = overallocate * page_size;

	void *region = mmap(nullptr, region_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
		throw std::runtime_error("memory mapping failed.\n");
	}

	madvise(region, region_size, MADV_RANDOM);

	std::vector<size_t> page_offsets(overallocate);
	std::iota(page_offsets.begin(), page_offsets.end(), 0);
	std::shuffle(page_offsets.begin(), page_offsets.end(), rng);

	std::vector<ll_node *> nodes(count);
	for (size_t i = 0; i < count; ++i) {
		nodes[i] = reinterpret_cast<ll_node *>(static_cast<char *>(region) + page_offsets[i] * page_size);
		nodes[i]->next = nullptr;
		nodes[i]->data = 0;
	}

	std::vector<size_t> positions(count);
	std::iota(positions.begin(), positions.end(), 0);
	std::shuffle(positions.begin(), positions.end(), rng);

	for (size_t i = 0; i < count; ++i) {
		nodes[positions[i]]->next = nodes[positions[(i + 1) % count]];
	}

	return {nodes, region, region_size};
}

template <typename Fn> static double time_cycle(const random_cycle &cycle, size_t set_size, Fn per_node)
{
	Measurements::Timer timer;

	ll_node *p = cycle.nodes[0];
	timer.TimeSnapshot();

	for (size_t i = 0; i < set_size; i++) {
		per_node(p, i);
		p = p->next;
	}

	volatile uintptr_t sink = (uintptr_t)p;
	return timer.GetNanoseconds() / (double)set_size;
}

static void run_test(size_t set_size, const char *label)
{
	assert((set_size == L1D_LIM || set_size == L2_LIM || set_size == L3_LIM) && "invalid set size.");

	random_cycle cycle = make_random_cycle(set_size);

	double read_ns = time_cycle(cycle, set_size, [](ll_node *, size_t) {});
	double write_ns = time_cycle(cycle, set_size, [](ll_node *p, size_t i) { p->data = i; });

	std::cout << label << " read time: " << read_ns << "ns | " << read_ns / 1000.0 << "us" << std::endl;
	std::cout << label << " write time: " << write_ns << "ns | " << write_ns / 1000.0 << "us" << std::endl;

	munmap(cycle.region, cycle.region_size);
}

int main()
{
	run_test(L1D_LIM, "L1D");
	run_test(L2_LIM, "L2");
	run_test(L3_LIM, "L3");

	return 0;
}
