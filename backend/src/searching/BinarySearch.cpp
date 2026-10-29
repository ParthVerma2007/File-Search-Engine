#include "BinarySearch.h"
int BinarySearch::search(const std::vector<std::string>& data, const std::string& key, std::size_t& c) {
    int low = 0, high = static_cast<int>(data.size()) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        ++c;
        if (data[mid] == key) return mid;
        if (data[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
