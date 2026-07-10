#include "DataRepository.h"
#include "CSVDataLoader.h"
#include "../common/TradingSession.h"
#include <algorithm>

namespace Origin {

void DataRepository::loadFile(const std::string& filepath, ProgressCallback callback) {
    CSVDataLoader loader;
    m_data = loader.loadWithProgress(filepath, callback);
    m_skippedLines = loader.getSkippedLineCount();

    // Group data into trading sessions
    m_tradingDays = TradingSession::groupBySession(m_data);

    // Extract filename from path
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        m_filename = filepath.substr(lastSlash + 1);
    } else {
        m_filename = filepath;
    }
}

OHLCVData DataRepository::getBarsForDays(int numDays) const {
    if (m_tradingDays.empty() || numDays <= 0) {
        return OHLCVData();
    }

    // Clamp to available days
    int availableDays = static_cast<int>(m_tradingDays.size());
    int daysToGet = std::min(numDays, availableDays);

    // Get the last N trading days
    int startDayIndex = availableDays - daysToGet;
    const TradingDay& firstDay = m_tradingDays[startDayIndex];
    const TradingDay& lastDay = m_tradingDays[availableDays - 1];

    // Extract bars from first day's start to last day's end
    OHLCVData result;
    result.reserve(lastDay.endIndex - firstDay.startIndex + 1);

    for (size_t i = firstDay.startIndex; i <= lastDay.endIndex && i < m_data.size(); ++i) {
        result.push_back(m_data[i]);
    }

    return result;
}

} // namespace Origin
