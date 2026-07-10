#pragma once

#include <string>

namespace Origin {

    struct SampleDataOptions {
        int tradingDays = 10;
        unsigned int seed = 42;
        double startPrice = 5000.0;
        double tickSize = 0.25;
    };

    // Write a synthetic 1-minute OHLCV CSV in the app's data format.
    // Mimics CME futures hours: sessions run 18:00 to 16:59 the next day,
    // the 17:00 maintenance hour and weekends are excluded.
    void writeSampleCsv(const std::string& filepath, const SampleDataOptions& options = {});

}
