#include "RadixSort.h"
#include <algorithm>
#include <cmath>
void RadixSort::sort(std::vector<double>& a){
    if(a.empty())return;
    // Convert non-negative relevance scores to fixed-point integers.
    std::vector<unsigned long long> x; x.reserve(a.size());
    for(double v:a)x.push_back((unsigned long long)std::llround(v*1000000.0));
    unsigned long long maxv=*std::max_element(x.begin(),x.end());
    for(unsigned long long exp=1;maxv/exp>0;exp*=10){
        std::vector<unsigned long long> out(x.size()); int cnt[10]={0};
        for(auto v:x)++cnt[(v/exp)%10];
        for(int i=1;i<10;++i)cnt[i]+=cnt[i-1];
        for(int i=(int)x.size()-1;i>=0;--i){auto d=(x[i]/exp)%10;out[--cnt[d]]=x[i];}
        x.swap(out);
        if(exp>maxv/10)break;
    }
    for(std::size_t i=0;i<a.size();++i)a[i]=x[i]/1000000.0;
}
