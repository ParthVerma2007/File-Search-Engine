#include "InsertionSort.h"
void InsertionSort::sort(std::vector<double>& a) {
    for(std::size_t i=1;i<a.size();++i){
        double key=a[i]; int j=static_cast<int>(i)-1;
        while(j>=0 && a[j]>key){ a[j+1]=a[j]; --j; }
        a[j+1]=key;
    }
}
