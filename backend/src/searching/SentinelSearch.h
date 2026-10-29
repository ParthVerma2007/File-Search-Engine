#pragma once
#include <vector>
#include <string>
#include <cstddef>

namespace SentinelSearch {
    int search(const std::vector<std::string>& data, const std::string& key, std::size_t& comparisons);
}
