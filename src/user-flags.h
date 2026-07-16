#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define HELP 0b10000000
#define NO_PREFETCHER 0b01000000

[[nodiscard]] uint16_t handle_flags(const int count, char **flags);
void print_help();
