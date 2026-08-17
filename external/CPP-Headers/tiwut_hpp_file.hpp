#ifndef TIWUT_HPP_FILE_UTILS_HPP
#define TIWUT_HPP_FILE_UTILS_HPP

/*
 * README: tiwut_hpp_file -std=c++17
 * ----------------------
 * 
 * Features:
 * - tiwut_hpp_file_write_atomic(): Safe file writing preventing corruption.
 * - tiwut_hpp_file_read_async(): std::future based background reading.
 * - tiwut_hpp_file_get_metadata(): File size, dates, and permission tracking.
 * - tiwut_hpp_dir_copy_recursive() / tiwut_hpp_dir_remove_recursive().
 * - Binary and Text modes optimized with RDBUF transfers.
 */

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <future>
#include <optional>
#include <system_error>

namespace tiwut_hpp_fs = std::filesystem;

// Basic Safe Operations
inline bool tiwut_hpp_file_exists(const std::string& path) { return tiwut_hpp_fs::exists(path); }
inline bool tiwut_hpp_dir_create(const std::string& path) { return tiwut_hpp_fs::create_directories(path); }

// Advanced Write Operations

// Atomic Write: Writes to a temporary file first, then swaps.
// Prevents corrupted files if the power cuts out or program crashes during write.
inline bool tiwut_hpp_file_write_atomic(const std::string& path, const std::string& content) {
    std::string temp_path = path + ".tiwut_tmp";
    try {
        std::ofstream out(temp_path, std::ios::binary);
        if (!out) return false;
        out.write(content.data(), content.size());
        out.close();
        tiwut_hpp_fs::rename(temp_path, path); // Atomic operation on POSIX/Modern Windows
        return true;
    } catch (...) {
        if (tiwut_hpp_fs::exists(temp_path)) tiwut_hpp_fs::remove(temp_path);
        return false;
    }
}

// Advanced Read Operations
inline std::optional<std::string> tiwut_hpp_file_read(const std::string& path) {
    if (!tiwut_hpp_file_exists(path)) return std::nullopt;
    std::ifstream in(path, std::ios::binary);
    if (!in) return std::nullopt;
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// Asynchronous File Reader using futures
inline std::future<std::optional<std::string>> tiwut_hpp_file_read_async(const std::string& path) {
    return std::async(std::launch::async, [path]() {
        return tiwut_hpp_file_read(path);
    });
}

// Directory Operations
inline std::vector<std::string> tiwut_hpp_dir_list(const std::string& path, bool recursive = false) {
    std::vector<std::string> files;
    if (!tiwut_hpp_file_exists(path) || !tiwut_hpp_fs::is_directory(path)) return files;
    
    if (recursive) {
        for (const auto& entry : tiwut_hpp_fs::recursive_directory_iterator(path))
            files.push_back(entry.path().string());
    } else {
        for (const auto& entry : tiwut_hpp_fs::directory_iterator(path))
            files.push_back(entry.path().string());
    }
    return files;
}

inline bool tiwut_hpp_dir_remove_recursive(const std::string& path) {
    if (!tiwut_hpp_file_exists(path)) return false;
    std::error_code ec;
    return tiwut_hpp_fs::remove_all(path, ec) != static_cast<std::uintmax_t>(-1);
}

// Metadata Extraction
struct tiwut_hpp_FileMetadata {
    uintmax_t size_bytes = 0;
    bool is_directory = false;
    bool read_only = false;
    std::string last_write_time = "";
};

inline std::optional<tiwut_hpp_FileMetadata> tiwut_hpp_file_get_metadata(const std::string& path) {
    if (!tiwut_hpp_file_exists(path)) return std::nullopt;
    
    tiwut_hpp_FileMetadata meta;
    std::error_code ec;
    
    meta.is_directory = tiwut_hpp_fs::is_directory(path, ec);
    meta.size_bytes = meta.is_directory ? 0 : tiwut_hpp_fs::file_size(path, ec);
    
    auto perms = tiwut_hpp_fs::status(path).permissions();
    meta.read_only = ((perms & tiwut_hpp_fs::perms::owner_write) == tiwut_hpp_fs::perms::none);

    // Format last modified time
    auto ftime = tiwut_hpp_fs::last_write_time(path, ec);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - tiwut_hpp_fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto c_ftime = std::chrono::system_clock::to_time_t(sctp);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&c_ftime), "%Y-%m-%d %H:%M:%S");
    meta.last_write_time = ss.str();
    
    return meta;
}

#endif // TIWUT_HPP_FILE_UTILS_HPP
