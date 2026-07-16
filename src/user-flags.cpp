#include "user-flags.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unordered_set>

static const std::string_view VF_HELP {"--help"};
static const std::string_view VF_NO_PREFETCHER {"--no-prefetcher"};

static void define_valid_flags(std::vector<std::string_view> &valid_flags_vector, std::string_view command_name)
{
	valid_flags_vector.emplace_back(command_name);
	valid_flags_vector.emplace_back(VF_HELP);
	valid_flags_vector.emplace_back(VF_NO_PREFETCHER);
}

[[nodiscard]] static uint16_t validate_flags(std::vector<std::string> &flags_vector)
{
	uint16_t flags_byte{};
	std::string_view command_name = flags_vector[0];
	std::vector<std::string_view> valid_flags;
	valid_flags.reserve(flags_vector.size());
	define_valid_flags(valid_flags, command_name);

	std::unordered_set<std::string> valid_flags_set(valid_flags.begin(), valid_flags.end());
	bool flags_valid = std::all_of(flags_vector.begin(), flags_vector.end(), [&valid_flags_set](const std::string &flag) { return valid_flags_set.count(flag); });

	if (!flags_valid) {
		std::cerr << "passed flags invalid. run with the --help flag\n";
		exit(1);
	}

	for (const auto &flag : flags_vector) {
		if (flag == VF_HELP) {
			flags_byte |= HELP;
		}
		else if (flag == VF_NO_PREFETCHER) {
			flags_byte |= NO_PREFETCHER;
		}
	}

	return flags_byte;
}

void handle_flags(char **flags, const size_t count)
{
	uint16_t passed_flags{};

	if (flags == nullptr)
		return;

	std::vector<std::string> str_flags;
	str_flags.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		str_flags.emplace_back(flags[i]);
	}

	passed_flags = validate_flags(str_flags);

	std::cout << passed_flags << std::endl;
}
