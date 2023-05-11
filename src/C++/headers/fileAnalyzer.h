#ifndef FILEANALYZER_H
#define FILEANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <QObject>
#include "RustPart.h"

class FileAnalyzer : public QObject {
    Q_OBJECT

public:
    FileAnalyzer() = default;
    ~FileAnalyzer();
    FileAnalyzer(const std::vector<std::string>& files);

    std::unordered_map<std::string, std::vector<std::string>> getResults();
    std::unordered_map<std::string, std::vector<std::string>> findDuplicates();
    std::unordered_map<std::string, DirectoryStats*> statsMap;

    uint64_t computeTotalSize();

    void analyzeFiles(const QStringList& files);

    DirectoryStats* getStats();

signals:
    void analysisFinished(const std::unordered_map<std::string, DirectoryStats*>& statsMap);

private:
    std::vector<std::string> files;
    std::unordered_map<std::string, std::vector<std::string>> results;

    DirectoryStats* stats;
};

#endif // FILEANALYZER_H
