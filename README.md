# cache-speed

Measures per-access read and write latency for L1d, L2, and L3 caches.

## How it works

**This benchmark assumes your CPU has a maximum 3 levels of cache.**

Each cache level is tested by allocating a working set of nodes sized to ~80% of the
cache's actual capacity (in cache lines) and walking a randomly-linked list through it.

## Usage

```
usage: cache-speed [--no-prefetcher] [--help]
  --help            prints help
  --no-prefetcher   disables prefetcher assistance during testing
```

## Example results

Measured on an AMD Ryzen 5 5600 (Zen 3). Reference values from [7-cpu.com](https://www.7-cpu.com/cpu/Zen3.html).

| Level | This benchmark | AMD spec (cycles) |
|-------|---------------|-------------------|
| L1D read | 2.04 ns (~9 cyc) | 4-5 cyc |
| L2 read | 2.90 ns (~13 cyc) | ~12 cyc |
| L3 read | 11.95 ns (~53 cyc) | ~46 cyc |

## Build and run

```
g++ -std=c++20 -O2 -o cache-speed src/*.cpp
./cache-speed
```

## Additional info

If you have a way to make this more optimized, feel free to contribute.
