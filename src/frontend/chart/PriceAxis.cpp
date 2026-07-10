#include "PriceAxis.h"
#include "../assets/Colors.h"
#include <cmath>
#include <cstdio>

namespace Origin {

void PriceAxis::render(HDC hdc) {
    if (!m_scaler) return;

    // Create font for labels
    HFONT font = CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, Colors::TextSecondary);
    SetBkMode(hdc, TRANSPARENT);

    // Calculate price range and interval
    double minPrice = m_scaler->getMinPrice();
    double maxPrice = m_scaler->getMaxPrice();
    double priceRange = maxPrice - minPrice;
    double interval = calculateNiceInterval(priceRange, DESIRED_TICKS);

    // Calculate starting price (rounded down to interval)
    double startPrice = roundToInterval(minPrice, interval);
    if (startPrice < minPrice) startPrice += interval;

    int chartLeft = m_scaler->getChartLeft();
    int chartRight = m_scaler->getChartRight();
    int labelX = chartRight + 5;

    // Draw price labels (no grid lines)
    for (double price = startPrice; price <= maxPrice; price += interval) {
        int y = m_scaler->priceToY(price);

        // Draw price label
        char label[32];
        sprintf_s(label, "%.2f", price);
        TextOutA(hdc, labelX, y - 6, label, static_cast<int>(strlen(label)));
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

double PriceAxis::calculateNiceInterval(double range, int desiredTicks) const {
    if (range <= 0 || desiredTicks <= 0) return 1.0;

    double rawInterval = range / desiredTicks;

    // Find the magnitude
    double magnitude = pow(10, floor(log10(rawInterval)));

    // Normalize to 1-10 range
    double normalized = rawInterval / magnitude;

    // Round to a nice number (1, 2, 2.5, 5, or 10)
    double niceNormalized;
    if (normalized <= 1.0) niceNormalized = 1.0;
    else if (normalized <= 2.0) niceNormalized = 2.0;
    else if (normalized <= 2.5) niceNormalized = 2.5;
    else if (normalized <= 5.0) niceNormalized = 5.0;
    else niceNormalized = 10.0;

    return niceNormalized * magnitude;
}

double PriceAxis::roundToInterval(double value, double interval) const {
    return floor(value / interval) * interval;
}

} // namespace Origin
