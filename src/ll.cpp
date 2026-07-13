#include "ll.h"
#include "vm.h"

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>

#elif defined(__linux__)
#include <sys/mman.h>
#include <sys/types.h>

#endif

random_cycle make_random_cycle(size_t count)
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
