#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Flag handling is all over the place, should be revised

// This macro is used to reserve space for valid_flags vector
// It should always be set at the total number of flags + 1 for the actual command name that is passed around argv
// This will probably be tedious to maintain, but for now it's okay
#define FLAG_COUNT 		3

#define HELP			0b10000000
#define NO_PREFETCHER		0b01000000

[[nodiscard]] uint16_t handle_flags(const int count, char **flags);
void print_help();
