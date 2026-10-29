#pragma once
#include <string>
#include <vector>
#include <unordered_set>

class Tokenizer {
public:
    Tokenizer();
    std::vector<std::string> process(const std::string& text) const;
    static std::string normalizeWord(std::string word);

private:
    std::unordered_set<std::string> stopWords_;
    static std::string stem(std::string word);
};
