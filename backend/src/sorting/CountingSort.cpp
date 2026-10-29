#include "CountingSort.h"
#include <cmath>
#include <algorithm>
void CountingSort::sort(std::vector<double>& a){
    if(a.empty())return;
    // Scores are non-negative. Scale to thousandths for a stable integer-key demonstration.
    std::vector<int> x; x.reserve(a.size());
    for(double v:a)x.push_back((int)std::llround(v*1000.0));
    int mn=*std::min_element(x.begin(),x.end()), mx=*std::max_element(x.begin(),x.end());
    if((long long)mx-mn>200000){ std::sort(a.begin(),a.end()); return; }
    std::vector<int> cnt(mx-mn+1);
    for(int v:x)++cnt[v-mn];
    std::size_t k=0;
    for(int i=0;i<(int)cnt.size();++i)while(cnt[i]--)a[k++]=(i+mn)/1000.0;
}
