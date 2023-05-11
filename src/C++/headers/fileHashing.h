#ifndef FILEHASHING_H
#define FILEHASHING_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

namespace FileHashing {
    std::string calculateSHA256(const std::string& path);
    std::unordered_map<std::string, std::vector<std::string>> findDuplicates(const std::vector<std::string>& files);
}

#endif
