#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define HELP 			0b10000000
#define NO_PREFETCHER 		0b01000000

void handle_flags(char **flags, const size_t count);
