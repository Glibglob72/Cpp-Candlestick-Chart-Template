#include "SampleDataGenerator.h"
#include "common/DateTime.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <random>

namespace Origin {

namespace {

// 2024-01-07 18:00:00 UTC — a Sunday evening, start of the Monday session
constexpr int64_t FIRST_SESSION_START = 1704650400;
constexpr int64_t SECONDS_PER_DAY = 86400;
constexpr int BARS_PER_SESSION = 1380;  // 18:00 through 16:59 the next day

bool isWeekendSession(int64_t sessionStart) {
    // Trading day = calendar day of (sessionStart + 6h). Epoch day 0
    // (1970-01-01) was a Thursday, so (dayIndex + 4) % 7 gives the weekday
    // with 0 = Sunday ... 6 = Saturday.
    int64_t weekday = ((sessionStart + 6 * 3600) / SECONDS_PER_DAY + 4) % 7;
    return weekday == 6 || weekday == 0;
}

} // namespace

void writeSampleCsv(const std::string& filepath, const SampleDataOptions& options) {
    std::ofstream out(filepath);
    out << "date,time,open,high,low,close,volume\n";

    std::mt19937 rng(options.seed);
    std::normal_distribution<double> step(0.0, 1.1);           // ticks per sub-step
    std::uniform_int_distribution<int> quietVolume(20, 400);
    std::uniform_int_distribution<int> activeVolume(400, 4000);

    // Random-walk the price in tick units, clamped away from zero
    double priceTicks = options.startPrice / options.tickSize;
    const double floorTicks = std::max(1.0, priceTicks * 0.5);

    auto toPrice = [&](double ticks) { return std::round(ticks) * options.tickSize; };

    char line[128];
    int64_t sessionStart = FIRST_SESSION_START;
    for (int day = 0; day < options.tradingDays; sessionStart += SECONDS_PER_DAY) {
        if (isWeekendSession(sessionStart)) continue;

        for (int m = 0; m < BARS_PER_SESSION; ++m) {
            Timestamp ts = sessionStart + static_cast<int64_t>(m) * 60;

            double open = priceTicks;
            double high = open;
            double low = open;
            for (int s = 0; s < 4; ++s) {
                priceTicks = std::max(floorTicks, priceTicks + step(rng));
                high = std::max(high, priceTicks);
                low = std::min(low, priceTicks);
            }

            // Round to whole ticks, keeping high/low outside the body
            double o = toPrice(open);
            double c = toPrice(priceTicks);
            double h = std::max({toPrice(high), o, c});
            double l = std::min({toPrice(low), o, c});

            // Heavier volume during regular trading hours (09:30-16:00)
            int hour = DateTime::getHour(ts);
            int minute = DateTime::getMinute(ts);
            bool active = (hour > 9 || (hour == 9 && minute >= 30)) && hour < 16;
            int volume = active ? activeVolume(rng) : quietVolume(rng);

            std::snprintf(line, sizeof(line), "%s,%s,%.2f,%.2f,%.2f,%.2f,%d\n",
                          DateTime::formatDate(ts).c_str(),
                          DateTime::formatTime(ts).c_str(),
                          o, h, l, c, volume);
            out << line;
        }

        ++day;
    }
}

} // namespace Origin
