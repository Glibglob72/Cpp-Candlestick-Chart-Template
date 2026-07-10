#pragma once

#include "ChartScaler.h"
#include "../../common/OHLCVBar.h"
#include <windows.h>

namespace Origin {
    class TimeAxis {
    public:
        // Set the scaler to use for coordinate transformations
        void setScaler(const ChartScaler* scaler) { m_scaler = scaler; }

        // Set the data for time labels
        void setData(const OHLCVData* data) { m_data = data; }

        // Enable/disable 18:00 session lines
        void setSessionLinesEnabled(bool enabled) { m_sessionLinesEnabled = enabled; }
        bool getSessionLinesEnabled() const { return m_sessionLinesEnabled; }

        // Render the time axis at the bottom
        void render(HDC hdc);

        // Get the height needed for the axis labels
        static constexpr int getHeight() { return 25; }

    private:
        const ChartScaler* m_scaler = nullptr;
        const OHLCVData* m_data = nullptr;
        bool m_sessionLinesEnabled = true;
    };
}
