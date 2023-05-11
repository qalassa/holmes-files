mod fs_traversal;

use std::ffi::CStr;
use std::ffi::c_char;
use crate::fs_traversal::DirectoryStats;

#[no_mangle]
pub extern "C" fn analyze_directory(path: *const c_char, include_hidden: bool) -> *mut DirectoryStats {
    let c_str = unsafe { CStr::from_ptr(path) };
    let path = c_str.to_str().unwrap();

    let stats = Box::new(fs_traversal::analyze_directory(path, include_hidden));

    Box::into_raw(stats)
}

#[no_mangle]
pub extern "C" fn free_directory_stats(stats: *mut DirectoryStats) {
    if !stats.is_null() {
        unsafe { Box::from_raw(stats) };
    }
}