#pragma once

#include "../common/OHLCVBar.h"
#include <string>
#include <functional>

namespace Origin {
    class CSVDataLoader {
    public:
        using ProgressCallback = std::function<void(int percent)>;

        // Load CSV file and return OHLCV data
        OHLCVData load(const std::string& filepath);

        // Load with progress callback (for large files)
        OHLCVData loadWithProgress(const std::string& filepath, ProgressCallback callback);

        // Number of malformed lines skipped during the last load
        size_t getSkippedLineCount() const { return m_skippedLines; }

    private:
        // Parse a single CSV line into an OHLCVBar
        OHLCVBar parseLine(const std::string& line);

        // Trim whitespace from string
        void trimWhitespace(std::string& str);

        // Split string by delimiter
        std::vector<std::string> split(const std::string& str, char delimiter);

        size_t m_skippedLines = 0;
    };
}
