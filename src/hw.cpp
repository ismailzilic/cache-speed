#include "hw.h"

#if defined(_WIN32)

#include <windows.h>

cache_size hw_poll_cache_sizes()
{
	cache_size ret = {0};

	DWORD buffer_size = 0;
	GetLogicalProcessorInformation(nullptr, &buffer_size);

	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

	if (!GetLogicalProcessorInformation(buffer.data(), &buffer_size)) {
		return ret;
	}

	for (const auto &info : buffer) {
		if (info.Relationship != RelationCache)
			continue;

		const CACHE_DESCRIPTOR &cache = info.Cache;
		uint32_t size_kb = cache.Size / 1024;

		if (cache.Level == 1 && cache.Type == CacheData) {
			ret.L1 = size_kb;
		} else if (cache.Level == 2) {
			ret.L2 = size_kb;
		} else if (cache.Level == 3) {
			ret.L3 = size_kb;
		}
	}

	return ret;
}

#elif defined(__linux__)

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

static const char *const polling_path = "/sys/devices/system/cpu/cpu0/cache";

[[nodiscard]] static std::string hw_get_cache_info(const fs::path &path)
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

[[nodiscard]] static size_t hw_convert_to_num(const std::string &string)
{
	size_t ret{};

	try {
		ret = std::stoul(string);
	} catch (std::invalid_argument const &ex) {
		std::cout << "hw_convert_to_num() failed: " << ex.what() << '\n';
	} catch (std::out_of_range const &ex) {
		std::cout << "hw_convert_to_num() failed: " << ex.what() << '\n';
	}

	return ret;
}

static cache_size hw_prep_return(const entries_vector &entries)
{
	cache_size ret = {0};

	for (const auto &entry : entries) {
		if (entry.first == "1") {
			ret.L1 = hw_convert_to_num(entry.second);
		}
		else if (entry.first == "2") {
			ret.L2 = hw_convert_to_num(entry.second);
		}
		else if (entry.first == "3") {
			ret.L3 = hw_convert_to_num(entry.second);
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
					level = hw_get_cache_info(i->path());
				}

				if (i->path().filename().string() == "size") {
					size = hw_get_cache_info(i->path());
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

	return hw_prep_return(cache_entries);
}

#endif
