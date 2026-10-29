#include "Trie.h"
#include <cctype>

Trie::Trie() : root_(std::make_unique<Node>()) {}

void Trie::clear() { root_ = std::make_unique<Node>(); }

void Trie::insert(const std::string& word) {
    Node* node = root_.get();
    for (char c : word) {
        if (c < 'a' || c > 'z') continue;
        int idx = c - 'a';
        if (!node->child[idx]) node->child[idx] = std::make_unique<Node>();
        node = node->child[idx].get();
    }
    node->terminal = true;
}

void Trie::collect(const Node* node, std::string& current,
                   std::vector<std::string>& out, std::size_t limit) const {
    if (!node || out.size() >= limit) return;
    if (node->terminal) out.push_back(current);

    for (int i = 0; i < 26 && out.size() < limit; ++i) {
        if (!node->child[i]) continue;
        current.push_back(static_cast<char>('a' + i));
        collect(node->child[i].get(), current, out, limit);
        current.pop_back();
    }
}

std::vector<std::string> Trie::autocomplete(const std::string& prefix, std::size_t limit) const {
    const Node* node = root_.get();
    for (char c : prefix) {
        if (c < 'a' || c > 'z') return {};
        int idx = c - 'a';
        if (!node->child[idx]) return {};
        node = node->child[idx].get();
    }

    std::vector<std::string> out;
    std::string current = prefix;
    collect(node, current, out, limit);
    return out;
}
