#include "SentinelSearch.h"
int SentinelSearch::search(const std::vector<std::string>& input, const std::string& key, std::size_t& c) {
    if (input.empty()) return -1;
    auto data = input;
    data.push_back(key);
    std::size_t i = 0;
    while (true) {
        ++c;
        if (data[i] == key) break;
        ++i;
    }
    return i < input.size() ? static_cast<int>(i) : -1;
}
