#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace TestUtil {

// Writes content to a unique temp CSV file, deletes it on destruction
class TempCsvFile {
public:
    explicit TempCsvFile(const std::string& content) {
        static std::atomic<int> counter{0};
        auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() /
                 ("csvloader_test_" + std::to_string(stamp) + "_" +
                  std::to_string(counter++) + ".csv");
        std::ofstream out(m_path);
        out << content;
    }

    ~TempCsvFile() {
        std::error_code ec;
        std::filesystem::remove(m_path, ec);
    }

    std::string path() const { return m_path.string(); }

private:
    std::filesystem::path m_path;
};

inline const char* CSV_HEADER = "date,time,open,high,low,close,volume\n";

} // namespace TestUtil
