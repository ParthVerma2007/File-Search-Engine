#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef>

struct Document {
    int id = -1;
    std::string name;
    std::string path;
    std::string content;
    std::vector<std::string> tokens;
};

struct SearchResult {
    int documentId = -1;
    std::string filename;
    double score = 0.0;
    std::vector<std::string> matchingTerms;
    std::string snippet;
};

struct SearchStats {
    std::size_t documents = 0;
    std::size_t uniqueTerms = 0;
    std::size_t indexedTokens = 0;
    std::size_t comparisons = 0;
    double milliseconds = 0.0;
};
