#pragma once

#include "../common/OHLCVBar.h"
#include "../common/TradingSession.h"
#include <string>
#include <vector>
#include <functional>

namespace Origin {
    class DataRepository {
    public:
        using ProgressCallback = std::function<void(int percent)>;

        // Load data from a CSV file
        void loadFile(const std::string& filepath, ProgressCallback callback = nullptr);

        // Get all loaded bars
        const OHLCVData& getAllBars() const { return m_data; }

        // Get bars for a specific number of trading days (from the end)
        OHLCVData getBarsForDays(int numDays) const;

        // Get all trading day sessions
        const std::vector<TradingDay>& getTradingDays() const { return m_tradingDays; }

        // Get number of trading days available
        int getTotalDaysCount() const { return static_cast<int>(m_tradingDays.size()); }

        // Get the loaded filename (without path)
        std::string getLoadedFileName() const { return m_filename; }

        // Check if data is loaded
        bool hasData() const { return !m_data.empty(); }

        // Number of malformed lines skipped during the last load
        size_t getSkippedLineCount() const { return m_skippedLines; }

    private:
        OHLCVData m_data;
        std::vector<TradingDay> m_tradingDays;
        std::string m_filename;
        size_t m_skippedLines = 0;
    };
}
