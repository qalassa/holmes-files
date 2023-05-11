#include "fileHashing.h"
#include <sodium.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <vector>
#include <iomanip>

namespace FileHashing {
    std::string calculateSHA256(const std::string& path) {
        unsigned char hash[crypto_hash_sha256_BYTES];
        std::ifstream file(path, std::ios::binary);
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

        if (!file) {
            throw std::runtime_error("Could not open file: " + path);
        }

        crypto_hash_sha256(hash, buffer.data(), buffer.size());

        std::stringstream ss;
        for (int i = 0; i < crypto_hash_sha256_BYTES; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }

        return ss.str();
    }

    std::unordered_map<std::string, std::vector<std::string>> findDuplicates(const std::vector<std::string>& files) {
        std::unordered_map<std::string, std::vector<std::string>> hashToFilePaths;
        for (const auto& file : files) {
            try {
                std::string hash = calculateSHA256(file);
                hashToFilePaths[hash].push_back(file);
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error while calculating hash for file " << file << ": " << e.what() << std::endl;
            }
        }
        return hashToFilePaths;
    }

}
