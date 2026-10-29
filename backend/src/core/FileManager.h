#pragma once
#include "Types.h"
#include <filesystem>
#include <vector>

class FileManager {
public:
    explicit FileManager(std::filesystem::path directory);
    std::vector<Document> loadDocuments() const;

private:
    std::filesystem::path directory_;
};
