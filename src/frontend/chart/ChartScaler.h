#pragma once

#include "../../common/Types.h"

namespace Origin {
    class ChartScaler {
    public:
        // Set the viewport dimensions and margins
        void setViewport(int width, int height, int marginLeft, int marginRight,
                        int marginTop, int marginBottom);

        // Set the price range to display (adds 5% padding)
        void setPriceRange(Price minPrice, Price maxPrice);

        // Set the price range directly without padding (for drag operations)
        void setPriceRangeRaw(Price minPrice, Price maxPrice);

        // Set the number of bars to display
        void setBarCount(int barCount);

        // Convert price to Y coordinate
        int priceToY(Price price) const;

        // Convert bar index to X coordinate (left edge of bar)
        int barIndexToX(int index) const;

        // Get the width of each bar in pixels
        int getBarWidth() const;

        // Get the spacing between bars (0 when bars are very small)
        int getBarSpacing() const;

        // Get chart area dimensions
        int getChartLeft() const { return m_marginLeft; }
        int getChartRight() const { return m_width - m_marginRight; }
        int getChartTop() const { return m_marginTop; }
        int getChartBottom() const { return m_height - m_marginBottom; }
        int getChartWidth() const { return getChartRight() - getChartLeft(); }
        int getChartHeight() const { return getChartBottom() - getChartTop(); }

        // Get price range
        Price getMinPrice() const { return m_minPrice; }
        Price getMaxPrice() const { return m_maxPrice; }
        int getBarCount() const { return m_barCount; }

        // Convert Y coordinate back to price
        Price yToPrice(int y) const;

        // Scroll offset (in pixels)
        void setScrollOffset(int offset) { m_scrollOffset = offset; }
        int getScrollOffset() const { return m_scrollOffset; }

        // Get visible bar range based on scroll offset
        int getFirstVisibleBar() const;
        int getLastVisibleBar() const;

    private:
        int m_width = 0;
        int m_height = 0;
        int m_marginLeft = 0;
        int m_marginRight = 0;
        int m_marginTop = 0;
        int m_marginBottom = 0;

        Price m_minPrice = 0;
        Price m_maxPrice = 0;
        int m_barCount = 0;
        int m_scrollOffset = 0;
    };
}
