#include "TimeAxis.h"
#include "../assets/Colors.h"
#include "../../common/DateTime.h"
#include <cmath>
#include <cstdio>

namespace Origin {

void TimeAxis::render(HDC hdc) {
    if (!m_scaler || !m_data || m_data->empty()) return;

    int chartLeft = m_scaler->getChartLeft();
    int chartRight = m_scaler->getChartRight();
    int chartTop = m_scaler->getChartTop();
    int chartBottom = m_scaler->getChartBottom();
    int labelY = chartBottom + 5;

    // Draw session lines if enabled
    if (m_sessionLinesEnabled) {
        HPEN markerPen = CreatePen(PS_DOT, 1, Colors::MarkerLine);
        HPEN oldPen = (HPEN)SelectObject(hdc, markerPen);

        for (size_t i = 0; i < m_data->size(); ++i) {
            const OHLCVBar& bar = (*m_data)[i];
            int hour = DateTime::getHour(bar.timestamp);
            int minute = DateTime::getMinute(bar.timestamp);

            // Check if this bar is at 18:00
            if (hour == 18 && minute == 0) {
                int x = m_scaler->barIndexToX(static_cast<int>(i)) + m_scaler->getBarWidth() / 2;
                MoveToEx(hdc, x, chartTop, NULL);
                LineTo(hdc, x, chartBottom);
            }
        }

        SelectObject(hdc, oldPen);
        DeleteObject(markerPen);
    }

    // Draw time labels along the bottom axis
    HFONT font = CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, Colors::TextSecondary);
    SetBkMode(hdc, TRANSPARENT);

    // Calculate appropriate interval based on visible bars
    int barCount = m_scaler->getBarCount();
    int chartWidth = m_scaler->getChartWidth();
    if (barCount <= 0 || chartWidth <= 0) {
        SelectObject(hdc, oldFont);
        DeleteObject(font);
        return;
    }

    double pixelsPerBar = static_cast<double>(chartWidth) / barCount;
    int scrollOffset = m_scaler->getScrollOffset();

    // Determine label interval based on zoom level
    // We want labels roughly every 80-120 pixels
    int targetSpacing = 100;
    int barInterval = static_cast<int>(targetSpacing / pixelsPerBar);
    if (barInterval < 1) barInterval = 1;

    // Round to nice intervals (1, 5, 10, 15, 30, 60, etc. bars)
    int niceIntervals[] = {1, 5, 10, 15, 30, 60, 120, 240, 480, 1440};
    for (int interval : niceIntervals) {
        if (interval >= barInterval) {
            barInterval = interval;
            break;
        }
    }

    // Calculate first visible bar
    int firstBar = static_cast<int>((-scrollOffset) / pixelsPerBar);
    if (firstBar < 0) firstBar = 0;

    // Round to interval
    firstBar = (firstBar / barInterval) * barInterval;

    // Draw time labels
    for (int i = firstBar; i < static_cast<int>(m_data->size()); i += barInterval) {
        int x = m_scaler->barIndexToX(i) + m_scaler->getBarWidth() / 2;

        // Skip if outside visible area
        if (x < chartLeft - 50 || x > chartRight + 50) continue;

        const OHLCVBar& bar = (*m_data)[i];
        std::string timeStr = DateTime::formatTimeShort(bar.timestamp);

        // Check if we need to show date (at day boundaries or first label)
        bool showDate = false;
        int prevIndex = i - barInterval;
        if (i == firstBar || (prevIndex >= 0 && !DateTime::isSameDay(bar.timestamp, (*m_data)[prevIndex].timestamp))) {
            showDate = true;
        }

        SIZE textSize;
        if (showDate) {
            std::string dateStr = DateTime::formatDate(bar.timestamp).substr(5); // MM-DD format
            GetTextExtentPoint32A(hdc, dateStr.c_str(), static_cast<int>(dateStr.length()), &textSize);
            TextOutA(hdc, x - textSize.cx / 2, labelY, dateStr.c_str(), static_cast<int>(dateStr.length()));
        } else {
            GetTextExtentPoint32A(hdc, timeStr.c_str(), static_cast<int>(timeStr.length()), &textSize);
            TextOutA(hdc, x - textSize.cx / 2, labelY, timeStr.c_str(), static_cast<int>(timeStr.length()));
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

} // namespace Origin
