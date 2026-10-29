#include "FibonacciSearch.h"
int FibonacciSearch::search(const std::vector<std::string>& data, const std::string& key, std::size_t& c) {
    int n = static_cast<int>(data.size());
    int fibMMm2 = 0, fibMMm1 = 1, fibM = fibMMm2 + fibMMm1;
    while (fibM < n) {
        fibMMm2 = fibMMm1;
        fibMMm1 = fibM;
        fibM = fibMMm2 + fibMMm1;
    }

    int offset = -1;
    while (fibM > 1) {
        int i = std::min(offset + fibMMm2, n - 1);
        ++c;
        if (data[i] < key) {
            fibM = fibMMm1;
            fibMMm1 = fibMMm2;
            fibMMm2 = fibM - fibMMm1;
            offset = i;
        } else if (data[i] > key) {
            fibM = fibMMm2;
            fibMMm1 -= fibMMm2;
            fibMMm2 = fibM - fibMMm1;
        } else {
            return i;
        }
    }
    if (fibMMm1 && offset + 1 < n) {
        ++c;
        if (data[offset + 1] == key) return offset + 1;
    }
    return -1;
}
