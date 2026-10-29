#include "InvertedIndex.h"
#include <algorithm>

void InvertedIndex::clear() {
    index_.clear();
    tokenCount_ = 0;
}

void InvertedIndex::addDocument(const Document& doc) {
    for (const auto& token : doc.tokens) {
        ++tokenCount_;
        auto& posting = index_[token];
        if (posting.empty() || posting.back() != doc.id)
            posting.push_back(doc.id);
    }
}

const std::vector<int>& InvertedIndex::postings(const std::string& term) const {
    static const std::vector<int> empty;
    auto it = index_.find(term);
    return it == index_.end() ? empty : it->second;
}

bool InvertedIndex::contains(const std::string& term) const {
    return index_.find(term) != index_.end();
}

std::size_t InvertedIndex::uniqueTerms() const { return index_.size(); }
std::size_t InvertedIndex::indexedTokens() const { return tokenCount_; }

std::vector<std::string> InvertedIndex::sample(std::size_t limit) const {
    std::vector<std::string> words;
    words.reserve(index_.size());
    for (const auto& [word, _] : index_) words.push_back(word);
    std::sort(words.begin(), words.end());
    if (words.size() > limit) words.resize(limit);
    return words;
}

const std::unordered_map<std::string, std::vector<int>>& InvertedIndex::data() const {
    return index_;
}
