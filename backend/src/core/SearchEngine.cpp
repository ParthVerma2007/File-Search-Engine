#include "SearchEngine.h"
#include "../ranking/TFIDF.h"
#include "../ranking/CosineSimilarity.h"
#include "../searching/LinearSearch.h"
#include "../searching/SentinelSearch.h"
#include "../searching/BinarySearch.h"
#include "../searching/FibonacciSearch.h"
#include "../searching/IndexedSequentialSearch.h"
#include "../sorting/BubbleSort.h"
#include "../sorting/SelectionSort.h"
#include "../sorting/InsertionSort.h"
#include "../sorting/MergeSort.h"
#include "../sorting/QuickSort.h"
#include "../sorting/HeapSort.h"
#include "../sorting/CountingSort.h"
#include "../sorting/RadixSort.h"
#include "../sorting/BucketSort.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <sstream>
#include <unordered_set>

SearchEngine::SearchEngine(const std::filesystem::path& documentDir)
    : documentDir_(documentDir), fileManager_(documentDir) {
    rebuild();
}

void SearchEngine::rebuild() {
    auto start=std::chrono::high_resolution_clock::now();
    documents_=fileManager_.loadDocuments();
    index_.clear();
    trie_.clear();

    for(auto& doc:documents_){
        doc.tokens=tokenizer_.process(doc.content);
        index_.addDocument(doc);
        for(const auto& token:doc.tokens) trie_.insert(token);

        // Also insert normalized document words into Trie for natural autocomplete suggestions
        std::string word;
        for(unsigned char c : doc.content) {
            if(std::isalnum(c)) {
                word.push_back(static_cast<char>(std::tolower(c)));
            } else {
                if(word.size() >= 2) trie_.insert(word);
                word.clear();
            }
        }
        if(word.size() >= 2) trie_.insert(word);
    }

    indexStats_.documents=documents_.size();
    indexStats_.uniqueTerms=index_.uniqueTerms();
    indexStats_.indexedTokens=index_.indexedTokens();
    indexStats_.comparisons=0;
    indexStats_.milliseconds=std::chrono::duration<double,std::milli>(
        std::chrono::high_resolution_clock::now()-start).count();
}

std::vector<std::string> SearchEngine::queryTokens(const std::string& query) {
    Tokenizer t;
    return t.process(query);
}

std::string SearchEngine::makeSnippet(const Document& doc,const std::vector<std::string>& terms) {
    std::string text=doc.content;
    std::string lower=text;
    std::transform(lower.begin(),lower.end(),lower.begin(),
                   [](unsigned char c){return (char)std::tolower(c);});

    std::size_t pos=std::string::npos;
    for(const auto& term:terms){
        pos=lower.find(term);
        if(pos!=std::string::npos)break;
    }
    if(pos==std::string::npos)pos=0;
    std::size_t start=pos>60?pos-60:0;
    std::string s=text.substr(start,180);
    if(start>0)s="..."+s;
    if(start+s.size()<text.size())s+="...";
    return s;
}

std::vector<SearchResult> SearchEngine::search(const std::string& query,
                                                const std::string& searchAlgorithm,
                                                const std::string& sortAlgorithm,
                                                SearchStats& stats) const {
    auto start=std::chrono::high_resolution_clock::now();
    stats={};
    stats.documents=documents_.size();
    auto terms=queryTokens(query);
    if(terms.empty()){
        std::string rawNorm = Tokenizer::normalizeWord(query);
        if(!rawNorm.empty()) {
            terms.push_back(rawNorm);
        } else {
            return {};
        }
    }

    struct ExpandedTerm {
        std::string term;
        double matchFactor; // 1.0 for exact, ratio for prefix/partial
    };

    // For each query term, find exact or potential matching index terms
    std::unordered_map<std::string, std::vector<ExpandedTerm>> queryToMatches;
    std::unordered_set<int> candidateSet;

    for (const auto& qt : terms) {
        std::vector<ExpandedTerm>& matchedList = queryToMatches[qt];
        std::unordered_set<std::string> seen;

        // 1. Exact match in inverted index
        if (index_.contains(qt)) {
            matchedList.push_back({qt, 1.0});
            seen.insert(qt);
            for (int id : index_.postings(qt)) candidateSet.insert(id);
        }

        // 2. Prefix matching via Trie autocomplete (e.g. "algo" -> "algorithm", "algorithms")
        auto trieExpansions = trie_.autocomplete(qt, 25);
        for (const auto& exp : trieExpansions) {
            if (seen.count(exp)) continue;
            if (index_.contains(exp)) {
                double factor = (double)qt.length() / (double)exp.length();
                matchedList.push_back({exp, factor});
                seen.insert(exp);
                for (int id : index_.postings(exp)) candidateSet.insert(id);
            }
        }

        // 3. Fallback: Substring matching in indexed terms
        if (matchedList.empty() || qt.length() >= 3) {
            for (const auto& [idxTerm, posting] : index_.data()) {
                if (seen.count(idxTerm)) continue;
                if (idxTerm.find(qt) != std::string::npos) {
                    double factor = 0.8 * ((double)qt.length() / (double)idxTerm.length());
                    matchedList.push_back({idxTerm, factor});
                    seen.insert(idxTerm);
                    for (int id : posting) candidateSet.insert(id);
                } else if (qt.length() >= 4 && qt.find(idxTerm) != std::string::npos && idxTerm.length() >= 3) {
                    double factor = 0.75 * ((double)idxTerm.length() / (double)qt.length());
                    matchedList.push_back({idxTerm, factor});
                    seen.insert(idxTerm);
                    for (int id : posting) candidateSet.insert(id);
                }
            }
        }
    }

    if (candidateSet.empty()) return {};

    std::vector<int> candidates(candidateSet.begin(),candidateSet.end());
    std::sort(candidates.begin(),candidates.end());

    // Demonstrate the selected search algorithm against sorted document IDs.
    std::vector<std::string> searchable;
    searchable.reserve(documents_.size());
    for(const auto& d:documents_)searchable.push_back(std::to_string(d.id));
    std::sort(searchable.begin(),searchable.end());

    std::string probe=std::to_string(candidates.empty()?0:candidates.front());
    if(searchAlgorithm=="Sentinel Search") SentinelSearch::search(searchable,probe,stats.comparisons);
    else if(searchAlgorithm=="Binary Search") BinarySearch::search(searchable,probe,stats.comparisons);
    else if(searchAlgorithm=="Fibonacci Search") FibonacciSearch::search(searchable,probe,stats.comparisons);
    else if(searchAlgorithm=="Indexed Sequential Search") IndexedSequentialSearch::search(searchable,probe,stats.comparisons);
    else LinearSearch::search(searchable,probe,stats.comparisons);

    std::vector<SearchResult> results;
    const std::size_t N=documents_.size();

    for(int id:candidates){
        const Document& doc=documents_[id];
        std::vector<double> qv, dv;
        std::vector<std::string> matching;
        std::vector<std::string> snippetTerms = terms;
        double docMatchFactorSum = 0.0;
        int matchedTermCount = 0;

        for(const auto& qt:terms){
            const auto& matches = queryToMatches[qt];
            std::string bestTerm = "";
            double bestFactor = 0.0;
            double bestDocWeight = 0.0;

            for(const auto& em : matches){
                const auto& posting = index_.postings(em.term);
                if(std::find(posting.begin(), posting.end(), id) != posting.end()){
                    std::size_t df = posting.size();
                    double w = TFIDF::weight(doc, em.term, N, df);
                    if(em.matchFactor > bestFactor || (std::abs(em.matchFactor - bestFactor) < 1e-6 && w > bestDocWeight)){
                        bestFactor = em.matchFactor;
                        bestTerm = em.term;
                        bestDocWeight = w;
                    }
                }
            }

            if(!bestTerm.empty()){
                matching.push_back(bestTerm);
                snippetTerms.push_back(bestTerm);
                std::size_t df = index_.postings(bestTerm).size();
                double idf = TFIDF::inverseDocumentFrequency(N, df);
                qv.push_back(idf * bestFactor);
                dv.push_back(bestDocWeight);
                docMatchFactorSum += bestFactor;
                ++matchedTermCount;
            } else {
                qv.push_back(1.0);
                dv.push_back(0.0);
            }
        }

        double score=CosineSimilarity::calculate(qv,dv);
        if(!matching.empty()) score=std::min(1.0,score+0.15*(double)matching.size()/terms.size());

        // Modulate score based on prefix/partial match degree:
        // Exact matches receive full score; partial matches (like algo vs algorithm) reduce proportionally
        if(matchedTermCount > 0){
            double avgMatchFactor = docMatchFactorSum / matchedTermCount;
            score *= avgMatchFactor;
        }

        SearchResult r;
        r.documentId=id;
        r.filename=doc.name;
        r.score=score;
        r.matchingTerms=matching;
        r.snippet=makeSnippet(doc,snippetTerms);
        results.push_back(std::move(r));
    }

    std::vector<double> scores;
    for(const auto& r:results)scores.push_back(r.score);

    // Sort result indices through the selected syllabus sorting algorithm.
    // The result records are then reordered according to the sorted score order.
    std::vector<std::pair<double,int>> pairs;
    for(int i=0;i<(int)results.size();++i)pairs.push_back({results[i].score,i});

    // All sort implementations sort ascending, so reverse afterwards for relevance ranking.
    std::vector<double> sortKeys=scores;
    if(sortAlgorithm=="Bubble Sort") BubbleSort::sort(sortKeys);
    else if(sortAlgorithm=="Selection Sort") SelectionSort::sort(sortKeys);
    else if(sortAlgorithm=="Insertion Sort") InsertionSort::sort(sortKeys);
    else if(sortAlgorithm=="Merge Sort") MergeSort::sort(sortKeys);
    else if(sortAlgorithm=="Heap Sort") HeapSort::sort(sortKeys);
    else if(sortAlgorithm=="Counting Sort") CountingSort::sort(sortKeys);
    else if(sortAlgorithm=="Radix Sort") RadixSort::sort(sortKeys);
    else if(sortAlgorithm=="Bucket Sort") BucketSort::sort(sortKeys);
    else QuickSort::sort(sortKeys);

    std::vector<SearchResult> ranked;
    std::vector<bool> used(results.size(),false);
    for(auto it=sortKeys.rbegin();it!=sortKeys.rend();++it){
        for(std::size_t i=0;i<results.size();++i){
            if(!used[i] && std::abs(results[i].score-*it)<1e-10){
                ranked.push_back(results[i]);
                used[i]=true;
                break;
            }
        }
    }

    stats.uniqueTerms=index_.uniqueTerms();
    stats.indexedTokens=index_.indexedTokens();
    stats.milliseconds=std::chrono::duration<double,std::milli>(
        std::chrono::high_resolution_clock::now()-start).count();
    return ranked;
}

std::vector<std::string> SearchEngine::autocomplete(const std::string& prefix) const {
    std::string p=Tokenizer::normalizeWord(prefix);
    return trie_.autocomplete(p);
}
