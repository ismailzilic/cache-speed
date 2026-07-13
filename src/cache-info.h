#pragma once

#include "ll.h"

#include <cstddef>

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
