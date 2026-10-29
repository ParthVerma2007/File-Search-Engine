#include "CosineSimilarity.h"
#include <cmath>
double CosineSimilarity::calculate(const std::vector<double>& a,const std::vector<double>& b){
    if(a.size()!=b.size()||a.empty())return 0.0;
    double dot=0,na=0,nb=0;
    for(std::size_t i=0;i<a.size();++i){dot+=a[i]*b[i];na+=a[i]*a[i];nb+=b[i]*b[i];}
    if(na==0||nb==0)return 0.0;
    return dot/(std::sqrt(na)*std::sqrt(nb));
}
