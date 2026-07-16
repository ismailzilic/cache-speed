#include "benchmark.h"
#include "user-flags.h"

int main(int argc, char **argv)
{
	uint16_t flags = handle_flags(argc, argv);
	delegate_tests(flags);

	return 0;
}
