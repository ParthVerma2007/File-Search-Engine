#pragma once
#include <vector>
#include <string>
#include <cstddef>

namespace SearchCommon {
    inline void count(std::size_t& comparisons) { ++comparisons; }

    template <typename T>
    int binary(const std::vector<T>& a, const T& key, std::size_t& comparisons) {
        int low = 0, high = static_cast<int>(a.size()) - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            ++comparisons;
            if (a[mid] == key) return mid;
            if (a[mid] < key) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }
}
