#include "CandleRenderer.h"
#include "../assets/Colors.h"
#include <algorithm>

namespace Origin {

void CandleRenderer::render(HDC hdc, const OHLCVData& bars) {
    if (!m_scaler || bars.empty()) return;

    for (size_t i = 0; i < bars.size(); ++i) {
        drawCandle(hdc, bars[i], static_cast<int>(i));
    }
}

void CandleRenderer::drawCandle(HDC hdc, const OHLCVBar& bar, int index) {
    int barWidth = m_scaler->getBarWidth();
    int x = m_scaler->barIndexToX(index);
    int centerX = x + barWidth / 2;

    // Calculate Y coordinates (note: Y is inverted in screen coordinates)
    int highY = m_scaler->priceToY(bar.high);
    int lowY = m_scaler->priceToY(bar.low);
    int openY = m_scaler->priceToY(bar.open);
    int closeY = m_scaler->priceToY(bar.close);

    int bodyTop = std::min(openY, closeY);
    int bodyBottom = std::max(openY, closeY);

    // Ensure minimum body height of 1 pixel for doji candles
    if (bodyTop == bodyBottom) {
        bodyBottom = bodyTop + 1;
    }

    COLORREF color = bar.isBullish() ? Colors::BullishBody : Colors::BearishBody;

    // Draw wick (thin vertical line from high to low)
    HPEN wickPen = CreatePen(PS_SOLID, 1, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, wickPen);

    // Upper wick (from high to body top)
    MoveToEx(hdc, centerX, highY, NULL);
    LineTo(hdc, centerX, bodyTop);

    // Lower wick (from body bottom to low)
    MoveToEx(hdc, centerX, bodyBottom, NULL);
    LineTo(hdc, centerX, lowY + 1);  // +1 to include the low point

    SelectObject(hdc, oldPen);
    DeleteObject(wickPen);

    // Draw body (filled rectangle)
    int bodyWidth = std::max(1, barWidth - 2);  // Leave 1px gap between candles
    int bodyLeft = centerX - bodyWidth / 2;
    int bodyRight = bodyLeft + bodyWidth;

    HBRUSH bodyBrush = CreateSolidBrush(color);
    RECT bodyRect = { bodyLeft, bodyTop, bodyRight, bodyBottom };
    FillRect(hdc, &bodyRect, bodyBrush);
    DeleteObject(bodyBrush);
}

} // namespace Origin
