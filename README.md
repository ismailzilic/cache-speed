# cache-speed

Measures per-access read and write latency for L1d, L2, and L3 caches.

## How it works

Each cache level is tested by allocating a working set sized to ~80% of the cache's
actual capacity and walking a randomly-linked list through it. The 80% target avoids
eviction behavior that occurs when a cache is filled to exactly 100%

The number of loop repetitions per cache level is tuned so that each measurement takes
roughly 1-2 seconds of wall time, which is enough to average out OS jitter and other
noise while keeping the total benchmark runtime short.

## Build

```
g++ -std=c++17 -O2 -o cache-speed src/test.cpp src/vm.cpp src/timer.cpp
```

## Run

```
./cache-speed
```

## Todos

1. Calculate repeat counts at runtime
3. Logic for detecting cache sizes and recalculating them to be at ~80%
3. Get optional user inputs for cache sizes

## Additional info

If you have a way to make this more optimized, feel free to contribute.
