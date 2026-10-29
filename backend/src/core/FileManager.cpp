#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

FileManager::FileManager(std::filesystem::path directory)
    : directory_(std::move(directory)) {}

std::vector<Document> FileManager::loadDocuments() const {
    std::vector<Document> docs;
    if (!std::filesystem::exists(directory_)) {
        std::filesystem::create_directories(directory_);
        return docs;
    }

    std::vector<std::filesystem::path> paths;
    for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            paths.push_back(entry.path());
        }
    }

    std::sort(paths.begin(), paths.end());

    int id = 0;
    for (const auto& path : paths) {
        std::ifstream in(path);
        if (!in) continue;

        std::ostringstream buffer;
        buffer << in.rdbuf();

        Document doc;
        doc.id = id++;
        doc.name = path.filename().string();
        doc.path = path.string();
        doc.content = buffer.str();
        docs.push_back(std::move(doc));
    }
    return docs;
}
