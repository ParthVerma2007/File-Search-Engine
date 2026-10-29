#include "MergeSort.h"
static void merge(std::vector<double>& a,int l,int m,int r){
    std::vector<double> t; t.reserve(r-l+1);
    int i=l,j=m+1;
    while(i<=m && j<=r) t.push_back(a[i]<=a[j]?a[i++]:a[j++]);
    while(i<=m)t.push_back(a[i++]);
    while(j<=r)t.push_back(a[j++]);
    for(int k=0;k<(int)t.size();++k)a[l+k]=t[k];
}
static void rec(std::vector<double>& a,int l,int r){
    if(l>=r)return; int m=l+(r-l)/2;
    rec(a,l,m);rec(a,m+1,r);merge(a,l,m,r);
}
void MergeSort::sort(std::vector<double>& a){if(!a.empty())rec(a,0,(int)a.size()-1);}
