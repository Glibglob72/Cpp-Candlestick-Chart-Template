#include "frontend/chart/ChartScaler.h"
#include <gtest/gtest.h>

using namespace Origin;

namespace {

// 800x600 viewport with 60px left, 80px right, 20px top, 40px bottom margins:
// chart area spans x [60, 720], y [20, 560]
ChartScaler makeScaler() {
    ChartScaler scaler;
    scaler.setViewport(800, 600, 60, 80, 20, 40);
    return scaler;
}

} // namespace

TEST(ChartScalerTest, ViewportGeometry) {
    ChartScaler scaler = makeScaler();
    EXPECT_EQ(scaler.getChartLeft(), 60);
    EXPECT_EQ(scaler.getChartRight(), 720);
    EXPECT_EQ(scaler.getChartTop(), 20);
    EXPECT_EQ(scaler.getChartBottom(), 560);
    EXPECT_EQ(scaler.getChartWidth(), 660);
    EXPECT_EQ(scaler.getChartHeight(), 540);
}

TEST(ChartScalerTest, PriceRangeRawMapsEndpointsToChartEdges) {
    ChartScaler scaler = makeScaler();
    scaler.setPriceRangeRaw(100.0, 200.0);
    EXPECT_EQ(scaler.priceToY(100.0), scaler.getChartBottom());
    EXPECT_EQ(scaler.priceToY(200.0), scaler.getChartTop());
}

TEST(ChartScalerTest, SetPriceRangeAddsFivePercentPadding) {
    ChartScaler scaler = makeScaler();
    scaler.setPriceRange(100.0, 200.0);
    EXPECT_DOUBLE_EQ(scaler.getMinPrice(), 95.0);
    EXPECT_DOUBLE_EQ(scaler.getMaxPrice(), 205.0);
}

TEST(ChartScalerTest, YToPriceRoundTrip) {
    ChartScaler scaler = makeScaler();
    scaler.setPriceRangeRaw(100.0, 200.0);
    int y = scaler.priceToY(150.0);
    EXPECT_NEAR(scaler.yToPrice(y), 150.0, 0.5);
}

TEST(ChartScalerTest, EqualMinMaxPriceDoesNotDivideByZero) {
    ChartScaler scaler = makeScaler();
    scaler.setPriceRangeRaw(100.0, 100.0);
    EXPECT_EQ(scaler.priceToY(100.0), scaler.getChartTop());
}

TEST(ChartScalerTest, BarIndexToXDistributesBarsAcrossChartWidth) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(10);
    // 660px / 10 bars = 66px per bar
    EXPECT_EQ(scaler.barIndexToX(0), 60);
    EXPECT_EQ(scaler.barIndexToX(1), 126);
    EXPECT_EQ(scaler.barIndexToX(9), 60 + 594);
}

TEST(ChartScalerTest, BarIndexToXAppliesScrollOffset) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(10);
    scaler.setScrollOffset(-100);
    EXPECT_EQ(scaler.barIndexToX(0), 60 - 100);
}

TEST(ChartScalerTest, ZeroBarCountFallsBackSafely) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(0);
    EXPECT_EQ(scaler.barIndexToX(5), scaler.getChartLeft());
    EXPECT_EQ(scaler.getBarWidth(), 1);
}

TEST(ChartScalerTest, BarWidthIsAtLeastOnePixel) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(1000000);  // far more bars than pixels
    EXPECT_EQ(scaler.getBarWidth(), 1);
}

TEST(ChartScalerTest, VisibleBarRangeWithNoScrollCoversAllBars) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(100);
    scaler.setScrollOffset(0);
    EXPECT_EQ(scaler.getFirstVisibleBar(), 0);
    EXPECT_EQ(scaler.getLastVisibleBar(), 99);
}

TEST(ChartScalerTest, VisibleBarRangeShiftsWithScroll) {
    ChartScaler scaler = makeScaler();
    scaler.setBarCount(100);
    // 660px / 100 bars = 6.6px per bar; scrolled 66px left = 10 bars
    scaler.setScrollOffset(-66);
    EXPECT_EQ(scaler.getFirstVisibleBar(), 10);
    EXPECT_EQ(scaler.getLastVisibleBar(), 99);
}
