#pragma once
#include "../core/Types.h"
#include <unordered_map>
#include <vector>
#include <string>

class InvertedIndex {
public:
    void clear();
    void addDocument(const Document& doc);
    const std::vector<int>& postings(const std::string& term) const;
    bool contains(const std::string& term) const;
    std::size_t uniqueTerms() const;
    std::size_t indexedTokens() const;
    std::vector<std::string> sample(std::size_t limit = 30) const;
    const std::unordered_map<std::string, std::vector<int>>& data() const;

private:
    std::unordered_map<std::string, std::vector<int>> index_;
    std::size_t tokenCount_ = 0;
};
