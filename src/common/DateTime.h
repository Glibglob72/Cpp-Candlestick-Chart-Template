#pragma once

#include "Types.h"
#include <string>

namespace Origin {
    class DateTime {
    public:
        // Parse date string "YYYY-MM-DD" and time string "HH:MM:SS" to Unix timestamp
        static Timestamp parse(const std::string& date, const std::string& time);

        // Format timestamp to date string "YYYY-MM-DD"
        static std::string formatDate(Timestamp ts);

        // Format timestamp to time string "HH:MM:SS"
        static std::string formatTime(Timestamp ts);

        // Format timestamp to short time string "HH:MM"
        static std::string formatTimeShort(Timestamp ts);

        // Get hour component (0-23)
        static int getHour(Timestamp ts);

        // Get minute component (0-59)
        static int getMinute(Timestamp ts);

        // Check if two timestamps are on the same calendar day
        static bool isSameDay(Timestamp a, Timestamp b);

        // Get the start of day (midnight) for a timestamp
        static Timestamp getStartOfDay(Timestamp ts);
    };
}
