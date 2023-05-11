extern crate walkdir;

use walkdir::WalkDir;
use std::{
    fs::metadata,
    collections::HashMap,
    path::Path
};
use std::path::PathBuf;

#[derive(Debug)]
pub struct DirectoryStats {
    pub file_types: HashMap<String, usize>,
    pub total_size: u64,
    pub type_size: HashMap<String, u64>,
    pub largest_file: Option<(u64, PathBuf)>,
    pub most_common_type: Option<(String, usize)>,
    pub least_common_type: Option<(String, usize)>,
    pub directory_count: usize,
}


pub fn analyze_directory(path: &str, include_hidden: bool) -> DirectoryStats {
    let mut file_types = HashMap::new();
    let mut total_size = 0;
    let mut type_size = HashMap::new();
    let mut largest_file = None;
    let mut directory_count = 0;

    for entry in WalkDir::new(path).into_iter().filter_map(Result::ok) {
        if !include_hidden && is_hidden(&entry) {
            continue;
        }

        if let Ok(metadata) = metadata(entry.path()) {
            if metadata.is_file() {
                let extension = Path::new(entry.path())
                    .extension()
                    .and_then(|os_str| os_str.to_str())
                    .unwrap_or("Unknown");

                *file_types.entry(extension.to_string()).or_insert(0) += 1;

                let entry_size = metadata.len();
                total_size += entry_size;

                *type_size.entry(extension.to_string()).or_insert(0) += entry_size;

                match largest_file {
                    None => largest_file = Some((entry_size, entry.path().to_path_buf())),
                    Some((size, _)) if entry_size > size => largest_file =
                        Some((entry_size, entry.path().to_path_buf())),
                    _ => (),
                }

            } else if metadata.is_dir() {
                directory_count += 1;
            }
        }
    }

    let most_common_type = file_types.iter().max_by_key(|&(_, count)| count).map(|(type_, count)| (type_.clone(), *count));
    let least_common_type = file_types.iter().min_by_key(|&(_, count)| count).map(|(type_, count)| (type_.clone(), *count));

    DirectoryStats {
        file_types,
        total_size,
        type_size,
        largest_file,
        most_common_type,
        least_common_type,
        directory_count,
    }
}

fn is_hidden(entry: &walkdir::DirEntry) -> bool {
    entry.file_name().to_str().map(|s| s.starts_with(".")).unwrap_or(false)
}