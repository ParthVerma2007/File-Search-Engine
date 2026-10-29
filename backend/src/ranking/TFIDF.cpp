#include "TFIDF.h"
#include <cmath>
#include <unordered_map>
double TFIDF::termFrequency(const Document& doc,const std::string& term){
    if(doc.tokens.empty())return 0.0;
    std::size_t count=0;
    for(const auto& t:doc.tokens)if(t==term)++count;
    return (double)count/doc.tokens.size();
}
double TFIDF::inverseDocumentFrequency(std::size_t totalDocs,std::size_t docsWithTerm){
    if(totalDocs==0||docsWithTerm==0)return 0.0;
    return std::log((double)totalDocs/(double)docsWithTerm);
}
double TFIDF::weight(const Document& doc,const std::string& term,std::size_t totalDocs,std::size_t docsWithTerm){
    return termFrequency(doc,term)*inverseDocumentFrequency(totalDocs,docsWithTerm);
}
