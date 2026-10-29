#pragma once
#include "Types.h"
#include "FileManager.h"
#include "../preprocessing/Tokenizer.h"
#include "../indexing/InvertedIndex.h"
#include "../indexing/Trie.h"
#include <filesystem>
#include <string>
#include <vector>

class SearchEngine {
public:
    explicit SearchEngine(const std::filesystem::path& documentDir);

    void rebuild();
    std::vector<SearchResult> search(const std::string& query,
                                     const std::string& searchAlgorithm,
                                     const std::string& sortAlgorithm,
                                     SearchStats& stats) const;
    std::vector<std::string> autocomplete(const std::string& prefix) const;

    const std::vector<Document>& documents() const { return documents_; }
    const InvertedIndex& index() const { return index_; }
    const SearchStats& indexStats() const { return indexStats_; }

private:
    std::filesystem::path documentDir_;
    FileManager fileManager_;
    Tokenizer tokenizer_;
    std::vector<Document> documents_;
    InvertedIndex index_;
    Trie trie_;
    SearchStats indexStats_;

    static std::vector<std::string> queryTokens(const std::string& query);
    static std::string makeSnippet(const Document& doc, const std::vector<std::string>& terms);
};
