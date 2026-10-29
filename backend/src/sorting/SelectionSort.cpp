#include "SelectionSort.h"
#include <utility>
void SelectionSort::sort(std::vector<double>& a) {
    for(std::size_t i=0;i<a.size();++i){
        std::size_t m=i;
        for(std::size_t j=i+1;j<a.size();++j) if(a[j]<a[m]) m=j;
        if(m!=i) std::swap(a[i],a[m]);
    }
}
