#include "LinearSearch.h"
int LinearSearch::search(const std::vector<std::string>& data, const std::string& key, std::size_t& c) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        ++c;
        if (data[i] == key) return static_cast<int>(i);
    }
    return -1;
}
