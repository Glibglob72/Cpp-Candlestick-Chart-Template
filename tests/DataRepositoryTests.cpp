#include "backend/DataRepository.h"
#include "TempCsvFile.h"
#include <gtest/gtest.h>

using namespace Origin;
using TestUtil::TempCsvFile;

namespace {
const char* HEADER = TestUtil::CSV_HEADER;
} // namespace

TEST(DataRepositoryTest, LoadFileExposesBarsAndFilename) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n");

    DataRepository repo;
    repo.loadFile(file.path());

    EXPECT_TRUE(repo.hasData());
    EXPECT_EQ(repo.getAllBars().size(), 1u);
}

TEST(DataRepositoryTest, ReportsSkippedLineCountFromLoader) {
    TempCsvFile file(std::string(HEADER) +
        "2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250\n"
        "not,a,valid,line\n"
        "2024-01-15,09:31:00,garbage,4810.00,4802.50,4809.25,980\n");

    DataRepository repo;
    repo.loadFile(file.path());

    EXPECT_EQ(repo.getSkippedLineCount(), 2u);
    EXPECT_EQ(repo.getAllBars().size(), 1u);
}
