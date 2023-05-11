#include "fileAnalyzer.h"
#include "RustPart.h"
#include "fileHashing.h"

using namespace FileHashing;

FileAnalyzer::FileAnalyzer(const std::vector<std::string>& files) : files(files) { }


FileAnalyzer::~FileAnalyzer() {
    if (stats != nullptr) {
        free_directory_stats(stats);
    }
}

std::unordered_map<std::string, std::vector<std::string>> FileAnalyzer::getResults() {
    return results;
}

DirectoryStats* FileAnalyzer::getStats() {
    return stats;
}

uint64_t FileAnalyzer::computeTotalSize() {
    uint64_t totalSize = 0;

    for (const auto& pair : statsMap) {
        totalSize += pair.second->total_size;
    }

    return totalSize;
}


std::unordered_map<std::string, std::vector<std::string>> FileAnalyzer::findDuplicates() {
    std::unordered_map<std::string, std::vector<std::string>> duplicates = FileHashing::findDuplicates(files);
    results = FileHashing::findDuplicates(files);

    for (const auto& pair : duplicates) {
        if (pair.second.size() > 1) {
            std::cout << "Duplicate files (" << pair.first << "): ";
            for (const auto& path : pair.second) {
                std::cout << path << " ";
            }

            // Call analyze_directory on the first file of each duplicate group
            DirectoryStats* stats = analyze_directory(pair.second[0].c_str(), true);

            if (stats == nullptr) {
                throw std::runtime_error("analyze_directory failed");
            }

            // Store the stats object for later processing
            statsMap[pair.second[0]] = stats;

            // Extract the total size
            uint64_t totalSize = stats->total_size;

            // Extract the largest file
            LargestFile* largestFile = stats->largest_file;

            // Extract the most common and least common types
            CommonType* mostCommonType = stats->most_common_type;
            CommonType* leastCommonType = stats->least_common_type;

            // Extract the number of directories
            size_t directoryCount = stats->directory_count;


            // Free the memory allocated by analyze_directory
            free_directory_stats(stats);

            std::cout << std::endl;
        }
    }

    return duplicates;
}

void FileAnalyzer::analyzeFiles(const QStringList& files) {
    // Convert QStringList to std::vector<std::string>
    std::vector<std::string> fileVector;
    for (const auto& file : files) {
        fileVector.push_back(file.toStdString());
    }

    // Set the files member variable
    this->files = fileVector;

    // Find duplicates
    auto duplicates = findDuplicates();

    for (const auto& pair : duplicates) {
        if (pair.second.size() > 1) {
            std::cout << "Duplicate files (" << pair.first << "): ";
            for (const auto& path : pair.second) {
                std::cout << path << " ";
            }
            std::cout << std::endl;
        }
    }

    // When done
    emit analysisFinished(statsMap);
}
