#include "backend/CSVDataLoader.h"
#include "TempCsvFile.h"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace Origin;
using TestUtil::TempCsvFile;

namespace {
const char* HEADER = TestUtil::CSV_HEADER;
} // namespace

TEST(CSVDataLoaderTest, LoadsValidFile) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n"
        "2024-01-15,09:31:00,4803.75,4810.00,4802.50,4809.25,980\n");

    CSVDataLoader loader;
    OHLCVData data = loader.load(file.path());

    ASSERT_EQ(data.size(), 2u);
    EXPECT_DOUBLE_EQ(data[0].open, 4800.25);
    EXPECT_DOUBLE_EQ(data[0].high, 4805.50);
    EXPECT_DOUBLE_EQ(data[0].low, 4799.00);
    EXPECT_DOUBLE_EQ(data[0].close, 4803.75);
    EXPECT_EQ(data[0].volume, 1250u);
    EXPECT_EQ(data[1].volume, 980u);
}

TEST(CSVDataLoaderTest, ThrowsOnMissingFile) {
    CSVDataLoader loader;
    EXPECT_THROW(loader.load("Z:\\does\\not\\exist.csv"), std::runtime_error);
}

TEST(CSVDataLoaderTest, FileWithOnlyHeaderYieldsNoBars) {
    TempCsvFile file(HEADER);
    CSVDataLoader loader;
    EXPECT_TRUE(loader.load(file.path()).empty());
}

TEST(CSVDataLoaderTest, TrimsWhitespaceAroundFields) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15, 09:30:00 , 4800.25 ,4805.50,4799.00,4803.75, 1250 \n");

    CSVDataLoader loader;
    OHLCVData data = loader.load(file.path());

    ASSERT_EQ(data.size(), 1u);
    EXPECT_DOUBLE_EQ(data[0].open, 4800.25);
    EXPECT_EQ(data[0].volume, 1250u);
}

TEST(CSVDataLoaderTest, SkipsMalformedLinesAndKeepsValidOnes) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n"
        "not,a,valid,line\n"
        "2024-01-15,09:31:00,garbage,4810.00,4802.50,4809.25,980\n"
        "2024-01-15,09:32:00,4809.25,4812.00,4808.00,4811.50,1100\n");

    CSVDataLoader loader;
    OHLCVData data = loader.load(file.path());

    ASSERT_EQ(data.size(), 2u);
    EXPECT_DOUBLE_EQ(data[0].open, 4800.25);
    EXPECT_DOUBLE_EQ(data[1].open, 4809.25);
}

TEST(CSVDataLoaderTest, CountsSkippedMalformedLines) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n"
        "not,a,valid,line\n"
        "2024-01-15,09:31:00,garbage,4810.00,4802.50,4809.25,980\n"
        "2024-01-15,09:32:00,4809.25,4812.00,4808.00,4811.50,1100\n");

    CSVDataLoader loader;
    loader.load(file.path());

    EXPECT_EQ(loader.getSkippedLineCount(), 2u);
}

TEST(CSVDataLoaderTest, SkippedLineCountIsZeroForCleanFile) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n");

    CSVDataLoader loader;
    loader.load(file.path());

    EXPECT_EQ(loader.getSkippedLineCount(), 0u);
}

TEST(CSVDataLoaderTest, SkippedLineCountResetsBetweenLoads) {
    TempCsvFile dirty(std::string(HEADER) + "not,a,valid,line\n");
    TempCsvFile clean(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n");

    CSVDataLoader loader;
    loader.load(dirty.path());
    EXPECT_EQ(loader.getSkippedLineCount(), 1u);

    loader.load(clean.path());
    EXPECT_EQ(loader.getSkippedLineCount(), 0u);
}
