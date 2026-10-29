#pragma once
#include "../core/Types.h"
#include <string>
#include <vector>

namespace TFIDF {
    double termFrequency(const Document& doc, const std::string& term);
    double inverseDocumentFrequency(std::size_t totalDocs, std::size_t docsWithTerm);
    double weight(const Document& doc, const std::string& term, std::size_t totalDocs, std::size_t docsWithTerm);
}
