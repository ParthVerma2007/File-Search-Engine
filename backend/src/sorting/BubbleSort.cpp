#include "BubbleSort.h"
#include <utility>
void BubbleSort::sort(std::vector<double>& a) {
    for (std::size_t i=0;i<a.size();++i) {
        bool swapped=false;
        for (std::size_t j=0;j+1<a.size()-i;++j)
            if (a[j]>a[j+1]) { std::swap(a[j],a[j+1]); swapped=true; }
        if(!swapped) break;
    }
}
