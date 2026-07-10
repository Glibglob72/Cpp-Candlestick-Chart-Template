#include "TradingSession.h"
#include "DateTime.h"

namespace Origin {

std::vector<TradingDay> TradingSession::groupBySession(const OHLCVData& data) {
    std::vector<TradingDay> sessions;
    if (data.empty()) return sessions;

    TradingDay currentSession;
    currentSession.sessionStart = data[0].timestamp;
    currentSession.startIndex = 0;
    currentSession.endIndex = 0;

    // Track the previous bar's hour to detect 18:00 boundary
    int prevHour = DateTime::getHour(data[0].timestamp);

    for (size_t i = 1; i < data.size(); ++i) {
        int hour = DateTime::getHour(data[i].timestamp);
        int minute = DateTime::getMinute(data[i].timestamp);

        // Detect new session: when we hit exactly 18:00
        // This happens when current hour is 18 and minute is 0,
        // and we came from a different hour (or previous bar was before 18:00)
        bool isNewSession = (hour == SESSION_START_HOUR && minute == SESSION_START_MINUTE && prevHour != SESSION_START_HOUR);

        if (isNewSession) {
            // Save previous session
            currentSession.sessionEnd = data[i - 1].timestamp;
            sessions.push_back(currentSession);

            // Start new session
            currentSession = TradingDay();
            currentSession.sessionStart = data[i].timestamp;
            currentSession.startIndex = i;
            currentSession.endIndex = i;
        }
        else {
            // Continue current session
            currentSession.endIndex = i;
        }

        prevHour = hour;
    }

    // Don't forget the last session
    currentSession.sessionEnd = data.back().timestamp;
    sessions.push_back(currentSession);

    return sessions;
}

bool TradingSession::isInSessionHours(int hour, int minute) {
    // Session runs from 18:00 to 16:59
    // This means: 18:00-23:59 OR 00:00-16:59
    if (hour >= SESSION_START_HOUR) {
        return true;  // 18:00 - 23:59
    }
    if (hour < SESSION_END_HOUR) {
        return true;  // 00:00 - 15:59
    }
    if (hour == SESSION_END_HOUR && minute <= SESSION_END_MINUTE) {
        return true;  // 16:00 - 16:59
    }
    return false;
}

} // namespace Origin
