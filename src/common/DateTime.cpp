#include "DateTime.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace Origin {

Timestamp DateTime::parse(const std::string& date, const std::string& time) {
    std::tm tm = {};

    // Parse date: YYYY-MM-DD
    int year, month, day;
    char dash1, dash2;
    std::istringstream dateStream(date);
    dateStream >> year >> dash1 >> month >> dash2 >> day;

    // Parse time: HH:MM:SS
    int hour, minute, second;
    char colon1, colon2;
    std::istringstream timeStream(time);
    timeStream >> hour >> colon1 >> minute >> colon2 >> second;

    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;  // Let the system determine DST

    return static_cast<Timestamp>(_mkgmtime(&tm));
}

std::string DateTime::formatDate(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (tm.tm_year + 1900) << "-"
        << std::setw(2) << (tm.tm_mon + 1) << "-"
        << std::setw(2) << tm.tm_mday;
    return oss.str();
}

std::string DateTime::formatTime(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tm.tm_hour << ":"
        << std::setw(2) << tm.tm_min << ":"
        << std::setw(2) << tm.tm_sec;
    return oss.str();
}

std::string DateTime::formatTimeShort(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << tm.tm_hour << ":"
        << std::setw(2) << tm.tm_min;
    return oss.str();
}

int DateTime::getHour(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);
    return tm.tm_hour;
}

int DateTime::getMinute(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);
    return tm.tm_min;
}

bool DateTime::isSameDay(Timestamp a, Timestamp b) {
    return getStartOfDay(a) == getStartOfDay(b);
}

Timestamp DateTime::getStartOfDay(Timestamp ts) {
    std::time_t time = static_cast<std::time_t>(ts);
    std::tm tm;
    gmtime_s(&tm, &time);
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return static_cast<Timestamp>(_mkgmtime(&tm));
}

} // namespace Origin
