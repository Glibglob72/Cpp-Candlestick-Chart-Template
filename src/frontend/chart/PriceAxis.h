#pragma once

#include "ChartScaler.h"
#include <windows.h>

namespace Origin {
    class PriceAxis {
    public:
        // Set the scaler to use for coordinate transformations
        void setScaler(const ChartScaler* scaler) { m_scaler = scaler; }

        // Render the price axis on the right side
        void render(HDC hdc);

        // Get the width needed for the axis labels
        static constexpr int getWidth() { return 70; }

    private:
        // Calculate a nice interval for tick marks
        double calculateNiceInterval(double range, int desiredTicks) const;

        // Round a value to the nearest interval
        double roundToInterval(double value, double interval) const;

        const ChartScaler* m_scaler = nullptr;
        static constexpr int DESIRED_TICKS = 8;
    };
}
