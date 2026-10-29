#include "Tokenizer.h"
#include <cctype>
#include <algorithm>

Tokenizer::Tokenizer() : stopWords_{
    "a","an","and","are","as","at","be","by","for","from","has","have",
    "he","in","is","it","its","of","on","or","that","the","this","to",
    "was","were","will","with","you","your","i","we","they","their"
} {}

std::string Tokenizer::stem(std::string word) {
    // Intentionally small/basic stemming for a second-year DSA project.
    if (word.size() > 5 && word.size() >= 3 && word.compare(word.size()-3, 3, "ing") == 0) {
        word.resize(word.size() - 3);
    } else if (word.size() > 4 && word.size() >= 2 && word.compare(word.size()-2, 2, "ed") == 0) {
        word.resize(word.size() - 2);
    } else if (word.size() > 4 && word.size() >= 2 && word.compare(word.size()-2, 2, "es") == 0) {
        word.resize(word.size() - 2);
    } else if (word.size() > 3 && word.back() == 's') {
        word.pop_back();
    }
    return word;
}

std::string Tokenizer::normalizeWord(std::string word) {
    std::string out;
    for (unsigned char c : word) {
        if (std::isalnum(c)) out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

std::vector<std::string> Tokenizer::process(const std::string& text) const {
    std::vector<std::string> result;
    std::string current;

    auto flush = [&]() {
        if (current.empty()) return;
        std::string word = normalizeWord(current);
        current.clear();
        if (word.empty() || stopWords_.count(word)) return;
        word = stem(word);
        if (!word.empty() && !stopWords_.count(word))
            result.push_back(word);
    };

    for (unsigned char c : text) {
        if (std::isalnum(c)) current.push_back(static_cast<char>(c));
        else flush();
    }
    flush();
    return result;
}
