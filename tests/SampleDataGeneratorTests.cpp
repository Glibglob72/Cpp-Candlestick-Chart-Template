#include "SampleDataGenerator.h"
#include "backend/CSVDataLoader.h"
#include "common/DateTime.h"
#include "common/TradingSession.h"
#include <gtest/gtest.h>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace Origin;

namespace {

// Bars per session: 18:00-23:59 (360) + 00:00-16:59 (1020)
constexpr int BARS_PER_SESSION = 1380;

class SampleDataGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = (std::filesystem::temp_directory_path() /
                  ("sample_gen_test_" + std::to_string(stamp) + ".csv")).string();
    }

    void TearDown() override {
        std::error_code ec;
        std::filesystem::remove(m_path, ec);
    }

    OHLCVData generateAndLoad(const SampleDataOptions& options) {
        writeSampleCsv(m_path, options);
        return m_loader.load(m_path);
    }

    std::string m_path;
    CSVDataLoader m_loader;
};

} // namespace

TEST_F(SampleDataGeneratorTest, ProducesRequestedNumberOfTradingSessions) {
    SampleDataOptions options;
    options.tradingDays = 3;

    OHLCVData data = generateAndLoad(options);

    EXPECT_EQ(data.size(), 3u * BARS_PER_SESSION);
    EXPECT_EQ(TradingSession::groupBySession(data).size(), 3u);
}

TEST_F(SampleDataGeneratorTest, OutputParsesWithNoSkippedLines) {
    SampleDataOptions options;
    options.tradingDays = 2;

    OHLCVData data = generateAndLoad(options);

    EXPECT_FALSE(data.empty());
    EXPECT_EQ(m_loader.getSkippedLineCount(), 0u);
}

TEST_F(SampleDataGeneratorTest, BarsAreInternallyConsistent) {
    SampleDataOptions options;
    options.tradingDays = 2;

    OHLCVData data = generateAndLoad(options);
    ASSERT_FALSE(data.empty());

    for (size_t i = 0; i < data.size(); ++i) {
        const OHLCVBar& bar = data[i];
        EXPECT_GE(bar.high, bar.bodyTop()) << "bar " << i;
        EXPECT_LE(bar.low, bar.bodyBottom()) << "bar " << i;
        EXPECT_GT(bar.low, 0.0) << "bar " << i;
        EXPECT_GE(bar.volume, 1u) << "bar " << i;
        if (i > 0) {
            EXPECT_GT(bar.timestamp, data[i - 1].timestamp) << "bar " << i;
        }
    }
}

TEST_F(SampleDataGeneratorTest, PricesAreTickAligned) {
    SampleDataOptions options;
    options.tradingDays = 1;
    options.tickSize = 0.25;

    OHLCVData data = generateAndLoad(options);
    ASSERT_FALSE(data.empty());

    auto isAligned = [](double price) {
        double ticks = price / 0.25;
        return std::abs(ticks - std::round(ticks)) < 1e-9;
    };

    for (const OHLCVBar& bar : data) {
        EXPECT_TRUE(isAligned(bar.open));
        EXPECT_TRUE(isAligned(bar.high));
        EXPECT_TRUE(isAligned(bar.low));
        EXPECT_TRUE(isAligned(bar.close));
    }
}

TEST_F(SampleDataGeneratorTest, SkipsMaintenanceHourAndWeekends) {
    SampleDataOptions options;
    options.tradingDays = 7;  // spans a weekend

    OHLCVData data = generateAndLoad(options);
    ASSERT_FALSE(data.empty());

    for (const OHLCVBar& bar : data) {
        EXPECT_NE(DateTime::getHour(bar.timestamp), 17);

        // Trading day = calendar day of (timestamp + 6h); must be Mon-Fri.
        // Epoch day 0 (1970-01-01) was a Thursday, so (dayIndex + 4) % 7
        // gives weekday with 0 = Sunday ... 6 = Saturday.
        int64_t weekday = ((bar.timestamp + 6 * 3600) / 86400 + 4) % 7;
        EXPECT_NE(weekday, 6) << "trading day falls on Saturday";
        EXPECT_NE(weekday, 0) << "trading day falls on Sunday";
    }
}

TEST_F(SampleDataGeneratorTest, SameSeedProducesIdenticalOutput) {
    SampleDataOptions options;
    options.tradingDays = 1;
    options.seed = 123;

    writeSampleCsv(m_path, options);
    std::stringstream first;
    first << std::ifstream(m_path).rdbuf();

    writeSampleCsv(m_path, options);
    std::stringstream second;
    second << std::ifstream(m_path).rdbuf();

    ASSERT_GT(first.str().size(), 100u);
    EXPECT_EQ(first.str(), second.str());
}
