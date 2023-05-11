#include <vector>
#include <iostream>
#include <stdexcept>
extern "C" {
#include "rustPart.h"
}
#include "fileHashing.h"

using namespace std;
using namespace FileHashing;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file1> [<file2> ...]" << endl;
        return 1;
    }

    vector<string> files(argv + 1, argv + argc);
    try {
        unordered_map<string, vector<string>> duplicates = findDuplicates(files);

        for (const auto& pair : duplicates) {
            if (pair.second.size() > 1) {
                cout << "Duplicate files (" << pair.first << "): ";
                for (const auto& path : pair.second) {
                    cout << path << " ";
                }

                // Call analyze_directory on the first file of each duplicate group
                DirectoryStats* stats = analyze_directory(pair.second[0].c_str(), true);

                if (stats == nullptr) {
                    throw runtime_error("analyze_directory failed");
                }

                // Free the memory allocated by analyze_directory
                free_directory_stats(stats);

                cout << endl;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}