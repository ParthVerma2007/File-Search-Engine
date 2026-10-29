#include "QuickSort.h"
#include <utility>
static int part(std::vector<double>& a,int l,int r){
    double p=a[r]; int i=l;
    for(int j=l;j<r;++j) if(a[j]<p) std::swap(a[i++],a[j]);
    std::swap(a[i],a[r]); return i;
}
static void rec(std::vector<double>& a,int l,int r){
    if(l>=r)return; int p=part(a,l,r);rec(a,l,p-1);rec(a,p+1,r);
}
void QuickSort::sort(std::vector<double>& a){if(!a.empty())rec(a,0,(int)a.size()-1);}
