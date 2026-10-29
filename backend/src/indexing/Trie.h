#pragma once
#include <array>
#include <memory>
#include <string>
#include <vector>

class Trie {
public:
    Trie();
    void clear();
    void insert(const std::string& word);
    std::vector<std::string> autocomplete(const std::string& prefix, std::size_t limit = 10) const;

private:
    struct Node {
        bool terminal = false;
        std::array<std::unique_ptr<Node>, 26> child{};
    };
    std::unique_ptr<Node> root_;

    void collect(const Node* node, std::string& current,
                 std::vector<std::string>& out, std::size_t limit) const;
};
