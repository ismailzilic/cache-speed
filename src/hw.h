#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

using entries_vector = std::vector<std::pair<std::string, std::string>>;

struct cache_size {
	uint32_t L1;
	uint32_t L2;
	uint32_t L3;
};

cache_size hw_poll_cache_sizes();
