#pragma once

#include "../../common/OHLCVBar.h"
#include "ChartScaler.h"
#include <windows.h>

namespace Origin {
    class CandleRenderer {
    public:
        // Set the scaler to use for coordinate transformations
        void setScaler(const ChartScaler* scaler) { m_scaler = scaler; }

        // Render all candlesticks
        void render(HDC hdc, const OHLCVData& bars);

    private:
        // Draw a single candlestick
        void drawCandle(HDC hdc, const OHLCVBar& bar, int index);

        const ChartScaler* m_scaler = nullptr;
    };
}
