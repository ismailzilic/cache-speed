# cache-speed

Measures per-access read and write latency for L1d, L2, and L3 caches.

## How it works

**This benchmark assumes your CPU has a maximum 3 levels of cache.**

Each cache level is tested by allocating a working set of nodes sized to ~80% of the
cache's actual capacity (in cache lines) and walking a randomly-linked list through it.

Each node is placed on its own 4 KB page, so the traversal defeats
hardware prefetchers and exercises the TLB at each cache level.

## Example results

Measured on an AMD Ryzen 5 5600 (4470 MHz max boost). Reference values from
[7-cpu.com](https://www.7-cpu.com/cpu/Zen3.html) for Zen 3.

| Level | This benchmark | Reference | Overhead |
|-------|---------------|-----------|----------|
| L1D read | 5.25 ns (~23 cyc) | 4 cyc (~0.9 ns) | ~5.8x |
| L2 read | 14.58 ns (~65 cyc) | 12 cyc (~2.7 ns) | ~5.4x |
| L3 read | 99.25 ns (~451 cyc) | 47 cyc (~10.5 ns) | ~9.5x |

## Build and run

```
g++ -std=c++17 -O2 -o cache-speed src/**.cpp
./cache-speed
```

## Additional info

If you have a way to make this more optimized, feel free to contribute.
