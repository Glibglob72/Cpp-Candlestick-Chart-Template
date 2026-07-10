#include "ChartScaler.h"
#include <algorithm>

namespace Origin {

void ChartScaler::setViewport(int width, int height, int marginLeft, int marginRight,
                              int marginTop, int marginBottom) {
    m_width = width;
    m_height = height;
    m_marginLeft = marginLeft;
    m_marginRight = marginRight;
    m_marginTop = marginTop;
    m_marginBottom = marginBottom;
}

void ChartScaler::setPriceRange(Price minPrice, Price maxPrice) {
    // Add a small padding to the price range
    Price range = maxPrice - minPrice;
    Price padding = range * 0.05;  // 5% padding
    m_minPrice = minPrice - padding;
    m_maxPrice = maxPrice + padding;
}

void ChartScaler::setPriceRangeRaw(Price minPrice, Price maxPrice) {
    m_minPrice = minPrice;
    m_maxPrice = maxPrice;
}

void ChartScaler::setBarCount(int barCount) {
    m_barCount = barCount;
}

int ChartScaler::priceToY(Price price) const {
    if (m_maxPrice == m_minPrice) return getChartTop();

    double ratio = (price - m_minPrice) / (m_maxPrice - m_minPrice);
    // Y is inverted (0 at top)
    return getChartBottom() - static_cast<int>(ratio * getChartHeight());
}

int ChartScaler::barIndexToX(int index) const {
    if (m_barCount <= 0) return getChartLeft();

    // Use floating-point math to properly distribute bars across available width
    // This ensures all bars fit exactly within the chart area
    double chartWidth = static_cast<double>(getChartWidth());
    double pixelsPerBar = chartWidth / m_barCount;

    return getChartLeft() + static_cast<int>(index * pixelsPerBar) + m_scrollOffset;
}

int ChartScaler::getBarSpacing() const {
    // No explicit spacing - distribution handled by barIndexToX
    return 0;
}

int ChartScaler::getBarWidth() const {
    if (m_barCount <= 0) return 1;

    // Calculate bar width based on pixels per bar
    // This matches the distribution in barIndexToX
    double chartWidth = static_cast<double>(getChartWidth());
    double pixelsPerBar = chartWidth / m_barCount;

    // Bar width is ~80% of available space per bar, minimum 1px
    int barWidth = static_cast<int>(pixelsPerBar * 0.8);
    return std::max(1, barWidth);
}

Price ChartScaler::yToPrice(int y) const {
    if (getChartHeight() == 0) return m_minPrice;

    double ratio = static_cast<double>(getChartBottom() - y) / getChartHeight();
    return m_minPrice + ratio * (m_maxPrice - m_minPrice);
}

int ChartScaler::getFirstVisibleBar() const {
    if (m_barCount <= 0) return 0;

    double chartWidth = static_cast<double>(getChartWidth());
    double pixelsPerBar = chartWidth / m_barCount;
    if (pixelsPerBar <= 0) return 0;

    int firstBar = static_cast<int>(-m_scrollOffset / pixelsPerBar);
    return std::max(0, firstBar);
}

int ChartScaler::getLastVisibleBar() const {
    if (m_barCount <= 0) return 0;

    double chartWidth = static_cast<double>(getChartWidth());
    double pixelsPerBar = chartWidth / m_barCount;
    if (pixelsPerBar <= 0) return m_barCount - 1;

    int lastBar = static_cast<int>((-m_scrollOffset + getChartWidth()) / pixelsPerBar) + 1;
    return std::min(m_barCount - 1, lastBar);
}

} // namespace Origin
