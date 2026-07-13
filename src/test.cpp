#include "benchmark.h"
#include "cache-info.h"

int main()
{
	run_test(L1D_CAP, "L1D");
	run_test(L2_CAP, "L2");
	run_test(L3_CAP, "L3");

	return 0;
}
