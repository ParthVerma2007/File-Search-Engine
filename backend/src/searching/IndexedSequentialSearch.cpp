#include "IndexedSequentialSearch.h"
#include <cmath>
#include <algorithm>

int IndexedSequentialSearch::search(const std::vector<std::string>& data, const std::string& key, std::size_t& c) {
    if (data.empty()) return -1;

    // Build a simple sparse index: every sqrt(n)th element.
    int n = static_cast<int>(data.size());
    int block = std::max(1, static_cast<int>(std::sqrt(n)));
    int start = 0;

    for (int i = block; i < n; i += block) {
        ++c;
        if (data[i] >= key) break;
        start = i;
    }

    int end = std::min(n, start + block + 1);
    for (int i = start; i < end; ++i) {
        ++c;
        if (data[i] == key) return i;
        if (data[i] > key) break;
    }
    return -1;
}
