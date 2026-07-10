#pragma once

#include "OHLCVBar.h"
#include <vector>

namespace Origin {
    // Represents a single trading day/session
    struct TradingDay {
        Timestamp sessionStart;   // 18:00 of start day
        Timestamp sessionEnd;     // 16:59 of next day
        size_t startIndex;        // First bar index in session
        size_t endIndex;          // Last bar index in session (inclusive)

        size_t barCount() const { return endIndex - startIndex + 1; }
    };

    class TradingSession {
    public:
        // Session timing constants
        static constexpr int SESSION_START_HOUR = 18;
        static constexpr int SESSION_START_MINUTE = 0;
        static constexpr int SESSION_END_HOUR = 16;
        static constexpr int SESSION_END_MINUTE = 59;

        // Group OHLCV bars into trading sessions
        // Each session runs from 18:00 to 16:59 the next calendar day
        static std::vector<TradingDay> groupBySession(const OHLCVData& data);

        // Check if a timestamp falls within session hours
        static bool isInSessionHours(int hour, int minute);
    };
}
