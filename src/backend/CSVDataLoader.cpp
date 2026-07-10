#include "CSVDataLoader.h"
#include "../common/DateTime.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Origin {

OHLCVData CSVDataLoader::load(const std::string& filepath) {
    return loadWithProgress(filepath, nullptr);
}

OHLCVData CSVDataLoader::loadWithProgress(const std::string& filepath, ProgressCallback callback) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    // Get file size for progress calculation
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    OHLCVData data;
    data.reserve(2000000);  // Reserve space for ~2M bars

    m_skippedLines = 0;

    std::string line;
    std::streamsize bytesRead = 0;
    int lastPercent = 0;

    // Skip header line
    if (std::getline(file, line)) {
        bytesRead += line.size() + 1;
    }

    while (std::getline(file, line)) {
        bytesRead += line.size() + 1;

        if (!line.empty()) {
            try {
                OHLCVBar bar = parseLine(line);
                data.push_back(bar);
            }
            catch (...) {
                // Skip malformed lines, but keep count so callers can report them
                ++m_skippedLines;
                continue;
            }
        }

        // Report progress
        if (callback && fileSize > 0) {
            int percent = static_cast<int>((bytesRead * 100) / fileSize);
            if (percent != lastPercent) {
                lastPercent = percent;
                callback(percent);
            }
        }
    }

    return data;
}

OHLCVBar CSVDataLoader::parseLine(const std::string& line) {
    std::vector<std::string> fields = split(line, ',');

    if (fields.size() < 7) {
        throw std::runtime_error("Invalid CSV line: not enough fields");
    }

    // Trim whitespace from all fields
    for (auto& field : fields) {
        trimWhitespace(field);
    }

    OHLCVBar bar;
    bar.timestamp = DateTime::parse(fields[0], fields[1]);
    bar.open = std::stod(fields[2]);
    bar.high = std::stod(fields[3]);
    bar.low = std::stod(fields[4]);
    bar.close = std::stod(fields[5]);
    bar.volume = std::stoull(fields[6]);

    return bar;
}

void CSVDataLoader::trimWhitespace(std::string& str) {
    // Trim leading whitespace
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        str.clear();
        return;
    }

    // Trim trailing whitespace
    size_t end = str.find_last_not_of(" \t\r\n");
    str = str.substr(start, end - start + 1);
}

std::vector<std::string> CSVDataLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

} // namespace Origin
