#include "hw.h"

#include <fstream>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

static const char const *polling_path = "/sys/devices/system/cpu/cpu0/cache";

// For debug
template <typename Tx, typename Ty> void print_container(const std::vector<std::pair<Tx, Ty>> &entries)
{
	for (const auto &ent : entries) {
		std::cout << "{ " << ent.first << ", " << ent.second << " }, ";
	}

	std::cout << '\n';
}

[[nodiscard]] static std::string hw_parse_cache_entry(const fs::path &path)
{
	std::string contents{};
	std::ifstream fstream(path);
	std::getline(fstream, contents);

	fstream.close();
	return contents;
}

static void hw_rm_dup_entries(entries_vector &entries)
{
	std::unordered_set<std::string> seen{};

	for (size_t i = 0; i < entries.size(); ++i) {

		// This check is a bit unreadable
		// insert() returns a std::pair<iterator, bool>
		// 'bool' (.second) indicates if passed key already existed in the unordered_map
		if (!seen.insert(entries[i].first).second)
			entries.erase(entries.begin() + i--);
	}
}

static void hw_clean_size_suffix(entries_vector &entries)
{
	for (size_t i = 0; i < entries.size(); ++i) {
		std::string_view temp{entries[i].second.substr(0, entries[i].second.size() - 1)};
		entries[i].second = temp;
	}
}

[[nodiscard]] static uint32_t hw_convert_to_num(const std::string &string)
{
	uint64_t scratch_val{};

	try {
		std::stoi(string, &scratch_val);
	} catch (std::invalid_argument const &ex) {
		std::cout << "hw_convert_to_num() failed: " << ex.what() << '\n';
	} catch (std::out_of_range const &ex) {
		std::cout << "hw_convert_to_num() failed: " << ex.what() << '\n';
	}

	return static_cast<uint32_t>(scratch_val);
}

static cache_size hw_prep_return(entries_vector &entries)
{
	cache_size ret = {0};

	std::vector<std::pair<uint32_t, uint32_t>> converted_values;
	converted_values.reserve(entries.size());

	for (size_t i = 0; i < entries.size(); ++i) {
		uint32_t level = hw_convert_to_num(entries[i].first);
		uint32_t size = hw_convert_to_num(entries[i].second);

		converted_values.emplace_back(level, size);
	}

	std::cout << "converted_values size: " << converted_values.size() << std::endl;
	std::cout << "printing integers...\n";
	print_container(converted_values);

	for (size_t i = 0; i < entries.size(); ++i) {
		switch (converted_values[i].first) {
		case 1:
			ret.L1 = converted_values[i].second;
			break;
		case 2:
			ret.L2 = converted_values[i].second;
			break;
		case 3:
			ret.L3 = converted_values[i].second;
			break;
		}
	}

	return ret;
}

cache_size hw_poll_cache_sizes()
{
	entries_vector cache_entries{};
	cache_entries.reserve(4);

	std::string level{};
	std::string size{};

	try {
		for (auto i = fs::recursive_directory_iterator(polling_path); i != fs::recursive_directory_iterator(); ++i) {
			if (i.depth() == 0 && !i->is_directory())
				continue;

			if (i->is_regular_file()) {
				if (i->path().filename().string() == "level") {
					level = hw_parse_cache_entry(i->path());
				}

				if (i->path().filename().string() == "size") {
					size = hw_parse_cache_entry(i->path());
				}
			}

			if (level.empty() || size.empty())
				continue;

			cache_entries.emplace_back(level, size);

			if (!level.empty() && !size.empty()) {
				level = "";
				size = "";
			}
		}

	} catch (fs::filesystem_error &error) {
		std::cerr << "filesystem error: " << error.what() << std::endl;
	}

	hw_rm_dup_entries(cache_entries);
	hw_clean_size_suffix(cache_entries);
	print_container(cache_entries);

	return hw_prep_return(cache_entries);
}
