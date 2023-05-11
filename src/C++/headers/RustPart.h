#ifndef RUSTPART_H
#define RUSTPART_H

#include <stdint.h>
#include <stdbool.h>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        const char* path;
        uint64_t size;
    } LargestFile;

    typedef struct {
        const char* type;
        size_t count;
    } CommonType;

    typedef struct {
        // TODO: Replace void* with the actual type of the map.
        void* file_types; // HashMap<String, usize>
        uint64_t total_size;
        void* type_size; // HashMap<String, u64>
        LargestFile* largest_file;
        CommonType* most_common_type;
        CommonType* least_common_type;
        size_t directory_count;
    } DirectoryStats;

    DirectoryStats* analyze_directory(const char* path, bool include_hidden);
    void free_directory_stats(DirectoryStats* stats);

#ifdef __cplusplus
}
#endif

#endif // RUSTPART_H