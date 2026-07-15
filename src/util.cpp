#include "util.h"

[[nodiscard]] size_t max_size_t(const size_t a, const size_t b) { return ((a >= b) ? a : b); }
