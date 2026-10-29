#include "HeapSort.h"
#include <utility>
static void heapify(std::vector<double>& a,int n,int i){
    int largest=i,l=2*i+1,r=2*i+2;
    if(l<n&&a[l]>a[largest])largest=l;
    if(r<n&&a[r]>a[largest])largest=r;
    if(largest!=i){std::swap(a[i],a[largest]);heapify(a,n,largest);}
}
void HeapSort::sort(std::vector<double>& a){
    int n=(int)a.size();
    for(int i=n/2-1;i>=0;--i)heapify(a,n,i);
    for(int i=n-1;i>0;--i){std::swap(a[0],a[i]);heapify(a,i,0);}
}
