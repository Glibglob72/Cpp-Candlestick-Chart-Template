#include "common/DateTime.h"
#include <gtest/gtest.h>

using namespace Origin;

TEST(DateTimeTest, ParsesDateAndTimeToUtcTimestamp) {
    // 2024-01-15 09:30:00 UTC
    Timestamp ts = DateTime::parse("2024-01-15", "09:30:00");
    EXPECT_EQ(ts, 1705311000);
}

TEST(DateTimeTest, ParseFormatRoundTrip) {
    Timestamp ts = DateTime::parse("2023-06-30", "17:59:59");
    EXPECT_EQ(DateTime::formatDate(ts), "2023-06-30");
    EXPECT_EQ(DateTime::formatTime(ts), "17:59:59");
    EXPECT_EQ(DateTime::formatTimeShort(ts), "17:59");
}

TEST(DateTimeTest, FormatPadsSingleDigitComponents) {
    Timestamp ts = DateTime::parse("2024-02-05", "04:07:09");
    EXPECT_EQ(DateTime::formatDate(ts), "2024-02-05");
    EXPECT_EQ(DateTime::formatTime(ts), "04:07:09");
}

TEST(DateTimeTest, GetHourAndMinute) {
    Timestamp ts = DateTime::parse("2024-01-15", "18:45:30");
    EXPECT_EQ(DateTime::getHour(ts), 18);
    EXPECT_EQ(DateTime::getMinute(ts), 45);
}

TEST(DateTimeTest, IsSameDayWithinOneDay) {
    Timestamp morning = DateTime::parse("2024-01-15", "00:00:00");
    Timestamp night = DateTime::parse("2024-01-15", "23:59:59");
    EXPECT_TRUE(DateTime::isSameDay(morning, night));
}

TEST(DateTimeTest, IsSameDayFalseAcrossMidnight) {
    Timestamp before = DateTime::parse("2024-01-15", "23:59:59");
    Timestamp after = DateTime::parse("2024-01-16", "00:00:00");
    EXPECT_FALSE(DateTime::isSameDay(before, after));
}

TEST(DateTimeTest, GetStartOfDayReturnsMidnight) {
    Timestamp ts = DateTime::parse("2024-01-15", "14:30:45");
    Timestamp midnight = DateTime::parse("2024-01-15", "00:00:00");
    EXPECT_EQ(DateTime::getStartOfDay(ts), midnight);
}
