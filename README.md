# cache-speed

Measures per-access read and write latency for L1d, L2, and L3 caches.

## Build

```
g++ -std=c++17 -O2 -o cache-speed src/test.cpp src/timer.cpp
```

## Run

```
./cache-speed
```

## Todos
1. Take user inputs for cache sizes
2. Make this Windows compatible
