#pragma once

#include <cstddef>
#include <vector>

struct ll_node {
	ll_node *next;
	size_t data;
};

struct random_cycle {
	std::vector<ll_node *> nodes;
	void *region;
	size_t region_size;
};

random_cycle make_random_cycle(size_t count);
