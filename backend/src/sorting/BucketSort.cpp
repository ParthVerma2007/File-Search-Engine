#include "BucketSort.h"
#include <algorithm>
void BucketSort::sort(std::vector<double>& a){
    if(a.empty())return;
    const std::size_t k=10;
    std::vector<std::vector<double>> buckets(k);
    for(double v:a){
        std::size_t idx=(std::size_t)std::min(9.0,std::max(0.0,v)*10.0);
        buckets[idx].push_back(v);
    }
    std::size_t p=0;
    for(auto& b:buckets){
        std::sort(b.begin(),b.end());
        for(double v:b)a[p++]=v;
    }
}
