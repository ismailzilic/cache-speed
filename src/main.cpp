#include "benchmark.h"
#include "hw.h"

int main()
{
	cache_size sizes = poll_cache_sizes();
	run_test(sizes.L1, "L1D");
	run_test(sizes.L2, "L2");
	run_test(sizes.L3, "L3");

	return 0;
}
