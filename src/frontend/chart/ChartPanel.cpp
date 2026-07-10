#include "ChartPanel.h"
#include "../assets/Colors.h"
#include "../../common/DateTime.h"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

namespace Origin {

void ChartPanel::registerClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = ChartPanel::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = NULL;  // We handle painting
    wc.lpszClassName = CLASS_NAME;
    wc.cbWndExtra = sizeof(ChartPanel*);

    RegisterClassExW(&wc);
}

LRESULT CALLBACK ChartPanel::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ChartPanel* panel = reinterpret_cast<ChartPanel*>(GetWindowLongPtr(hwnd, 0));

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            panel = reinterpret_cast<ChartPanel*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(panel));
            panel->m_hwnd = hwnd;
            panel->onCreate();
            return 0;
        }

        case WM_DESTROY:
            if (panel) panel->onDestroy();
            return 0;

        case WM_PAINT: {
            if (panel) panel->onPaint();
            return 0;
        }

        case WM_SIZE:
            if (panel) panel->onSize(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_MOUSEMOVE:
            if (panel) panel->onMouseMove(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_MOUSELEAVE:
            if (panel) panel->onMouseLeave();
            return 0;

        case WM_LBUTTONDOWN:
            if (panel) panel->onLeftButtonDown(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_LBUTTONUP:
            if (panel) panel->onLeftButtonUp(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_LBUTTONDBLCLK:
            if (panel) panel->onLeftButtonDoubleClick(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_RBUTTONDOWN:
            if (panel) panel->onRightButtonDown(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_RBUTTONUP:
            if (panel) panel->onRightButtonUp(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_ERASEBKGND:
            return 1;  // We handle erasing in WM_PAINT
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

ChartPanel::ChartPanel() {
    m_candleRenderer.setScaler(&m_scaler);
    m_priceAxis.setScaler(&m_scaler);
    m_timeAxis.setScaler(&m_scaler);
}

ChartPanel::~ChartPanel() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool ChartPanel::create(HWND parent, int x, int y, int width, int height) {
    m_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, width, height,
        parent,
        NULL,
        GetModuleHandle(NULL),
        this
    );

    return m_hwnd != nullptr;
}

void ChartPanel::setData(const OHLCVData& data) {
    m_data = data;
    m_timeAxis.setData(&m_data);
    calculatePriceRange();
    fitToData();
}

void ChartPanel::refresh() {
    if (m_hwnd) {
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

void ChartPanel::fitToData() {
    m_scaler.setScrollOffset(0);
    m_scaler.setBarCount(static_cast<int>(m_data.size()));
    calculatePriceRange();  // Reset to show full price range
    refresh();
}

void ChartPanel::setSessionLinesEnabled(bool enabled) {
    m_timeAxis.setSessionLinesEnabled(enabled);
    refresh();
}

void ChartPanel::onCreate() {
    // Try to initialize OpenGL
    if (m_useOpenGL) {
        if (m_glContext.initialize(m_hwnd)) {
            if (!m_glCandleRenderer.initialize()) {
                std::string message = "ChartPanel: OpenGL renderer init failed, "
                    "falling back to GDI. " + m_glCandleRenderer.getLastError() + "\n";
                OutputDebugStringA(message.c_str());
                m_glContext.shutdown();
                m_useOpenGL = false;
            } else {
                m_glCandleRenderer.setScaler(&m_scaler);
            }
        } else {
            OutputDebugStringA("ChartPanel: OpenGL context creation failed, "
                "falling back to GDI renderer\n");
            m_useOpenGL = false;
        }
    }
}

void ChartPanel::onDestroy() {
    if (m_useOpenGL) {
        m_glCandleRenderer.shutdown();
        m_glContext.shutdown();
    }
    if (m_memBitmap) {
        DeleteObject(m_memBitmap);
        m_memBitmap = nullptr;
    }
    if (m_memDC) {
        DeleteDC(m_memDC);
        m_memDC = nullptr;
    }
}

void ChartPanel::onPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    // Update scaler viewport
    m_scaler.setViewport(width, height,
                         10,                    // left margin
                         PriceAxis::getWidth(), // right margin for price axis
                         10,                    // top margin
                         TimeAxis::getHeight()); // bottom margin for time axis

    if (!m_data.empty()) {
        // Only reset bar count and price range if not actively dragging
        if (!m_isRightDragging && !m_isDragging) {
            if (m_scaler.getBarCount() == 0) {
                m_scaler.setBarCount(static_cast<int>(m_data.size()));
            }
            if (m_scaler.getMinPrice() == 0 && m_scaler.getMaxPrice() == 0) {
                calculatePriceRange();
            }
        }
    }

    // Always use GDI buffer for final compositing
    if (!m_memDC || width != m_bufferWidth || height != m_bufferHeight) {
        if (m_memBitmap) DeleteObject(m_memBitmap);
        if (m_memDC) DeleteDC(m_memDC);
        m_memDC = CreateCompatibleDC(hdc);
        m_memBitmap = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(m_memDC, m_memBitmap);
        m_bufferWidth = width;
        m_bufferHeight = height;
    }

    // Clear background
    drawBackground(m_memDC, clientRect);

    if (m_useOpenGL && m_glContext.isValid()) {
        // Render candles with OpenGL
        m_glContext.makeCurrent();
        glClearColor(12.0f/255.0f, 12.0f/255.0f, 12.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!m_data.empty()) {
            m_glCandleRenderer.render(m_data, width, height);
        }

        // Read OpenGL pixels back to GDI buffer (chart area only)
        glFlush();
        glFinish();

        int chartLeft = m_scaler.getChartLeft();
        int chartTop = m_scaler.getChartTop();
        int chartWidth = m_scaler.getChartWidth();
        int chartHeight = m_scaler.getChartHeight();

        if (chartWidth > 0 && chartHeight > 0) {
            std::vector<unsigned char> pixels(chartWidth * chartHeight * 4);
            glReadPixels(chartLeft, height - chartTop - chartHeight, chartWidth, chartHeight,
                        GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels.data());

            // Create DIB and copy to memDC
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = chartWidth;
            bmi.bmiHeader.biHeight = chartHeight; // positive = bottom-up
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            SetDIBitsToDevice(m_memDC, chartLeft, chartTop, chartWidth, chartHeight,
                             0, 0, 0, chartHeight, pixels.data(), &bmi, DIB_RGB_COLORS);
        }

        // Draw axes and overlays on top
        if (!m_data.empty()) {
            m_timeAxis.render(m_memDC);
            m_priceAxis.render(m_memDC);
            if (m_mouseInWindow) {
                drawCrosshair(m_memDC);
                drawInfoBox(m_memDC);
            }
        }

        // Single blit to screen
        BitBlt(hdc, 0, 0, width, height, m_memDC, 0, 0, SRCCOPY);
    } else {
        // GDI fallback path
        if (!m_memDC || width != m_bufferWidth || height != m_bufferHeight) {
            if (m_memBitmap) DeleteObject(m_memBitmap);
            if (m_memDC) DeleteDC(m_memDC);
            m_memDC = CreateCompatibleDC(hdc);
            m_memBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(m_memDC, m_memBitmap);
            m_bufferWidth = width;
            m_bufferHeight = height;
        }

        drawBackground(m_memDC, clientRect);

        if (!m_data.empty()) {
            m_timeAxis.render(m_memDC);
            m_priceAxis.render(m_memDC);
            m_candleRenderer.render(m_memDC, m_data);
            if (m_mouseInWindow) {
                drawCrosshair(m_memDC);
                drawInfoBox(m_memDC);
            }
        }

        BitBlt(hdc, 0, 0, width, height, m_memDC, 0, 0, SRCCOPY);
    }

    EndPaint(m_hwnd, &ps);

    // Notify frame rendered for FPS counter
    if (m_frameCallback) {
        m_frameCallback();
    }
}

void ChartPanel::onSize(int width, int height) {
    if (m_useOpenGL && m_glContext.isValid()) {
        m_glContext.resize(width, height);
    }
    refresh();
}

void ChartPanel::onMouseMove(int x, int y) {
    // Set up mouse tracking for WM_MOUSELEAVE
    if (!m_trackingMouse) {
        TRACKMOUSEEVENT tme = {};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hwnd;
        TrackMouseEvent(&tme);
        m_trackingMouse = true;
    }

    m_mouseX = x;
    m_mouseY = y;
    m_mouseInWindow = true;

    // Handle left-click dragging (2D panning - both X and Y axes)
    if (m_isDragging) {
        int deltaX = x - m_dragStartX;
        int deltaY = y - m_dragStartY;

        // X-axis panning (only adjust scroll offset, don't touch bar count)
        m_scaler.setScrollOffset(m_scrollOffsetAtDragStart + deltaX);

        // Y-axis panning - shift the price range based on vertical drag
        Price priceRange = m_maxPriceAtLeftDragStart - m_minPriceAtLeftDragStart;
        double chartHeight = static_cast<double>(m_scaler.getChartHeight());

        // Calculate how much price change corresponds to the Y drag
        // Dragging up (negative deltaY) should move the view up (show higher prices)
        // Dragging down (positive deltaY) should move the view down (show lower prices)
        Price priceDelta = (static_cast<double>(deltaY) / chartHeight) * priceRange;

        Price newMinPrice = m_minPriceAtLeftDragStart + priceDelta;
        Price newMaxPrice = m_maxPriceAtLeftDragStart + priceDelta;

        m_scaler.setPriceRangeRaw(newMinPrice, newMaxPrice);
    }

    // Handle right-click dragging (zooming)
    if (m_isRightDragging) {
        // Cancel right-drag if mouse moves outside chart area (into header/margins)
        int chartLeft = m_scaler.getChartLeft();
        int chartRight = m_scaler.getChartRight();
        int chartTop = m_scaler.getChartTop();
        int chartBottom = m_scaler.getChartBottom();

        if (x < chartLeft || x > chartRight || y < chartTop || y > chartBottom) {
            m_isRightDragging = false;
            ReleaseCapture();
            refresh();
            return;
        }

        int deltaX = x - m_rightDragStartX;
        int deltaY = y - m_rightDragStartY;

        // Zoom sensitivity: 50 pixels per zoom step (1.1x magnification)
        const int pixelsPerZoomStep = 50;
        const double zoomFactorPerStep = 1.1;

        // X-axis zoom: drag right = zoom in, drag left = zoom out
        // Zooming works by changing how many bars we display
        double xZoomSteps = static_cast<double>(deltaX) / pixelsPerZoomStep;
        double xZoomFactor = std::pow(zoomFactorPerStep, xZoomSteps);

        // Calculate new bar count (fewer bars = more zoomed in)
        int newBarCount = static_cast<int>(m_barCountAtDragStart / xZoomFactor);
        newBarCount = std::max(10, std::min(newBarCount, static_cast<int>(m_data.size()) * 4));
        m_scaler.setBarCount(newBarCount);

        // Adjust scroll offset to keep the zoom centered on the drag start point
        // When bar count changes, we need to adjust scroll to keep the center point stable
        double chartWidth = static_cast<double>(m_scaler.getChartWidth());
        double oldPixelsPerBar = (m_barCountAtDragStart > 0) ? chartWidth / m_barCountAtDragStart : 1.0;
        double newPixelsPerBar = (newBarCount > 0) ? chartWidth / newBarCount : 1.0;

        // Calculate what bar index was under the cursor at drag start
        double barAtCenter = (m_rightDragCenterX - m_scaler.getChartLeft() - m_scrollOffsetAtRightDragStart) / oldPixelsPerBar;

        // Calculate new scroll offset to keep that bar under the same screen position
        int newScrollOffset = static_cast<int>(m_rightDragCenterX - m_scaler.getChartLeft() - barAtCenter * newPixelsPerBar);
        m_scaler.setScrollOffset(newScrollOffset);

        // Y-axis zoom: drag up = zoom in, drag down = zoom out
        // Note: screen Y is inverted (up is negative delta)
        double yZoomSteps = static_cast<double>(-deltaY) / pixelsPerZoomStep;
        double yZoomFactor = std::pow(zoomFactorPerStep, yZoomSteps);

        // Calculate price at the cursor position at drag start
        Price priceRange = m_maxPriceAtDragStart - m_minPriceAtDragStart;
        double chartHeight = static_cast<double>(m_scaler.getChartHeight());
        double priceAtCenter = m_maxPriceAtDragStart -
            (static_cast<double>(m_rightDragCenterY - m_scaler.getChartTop()) / chartHeight) * priceRange;

        // Apply zoom factor to price range, centered on the cursor position
        Price newPriceRange = priceRange / yZoomFactor;

        // Calculate relative position of cursor in the original range (0 = top/max, 1 = bottom/min)
        double relativePos = (m_maxPriceAtDragStart - priceAtCenter) / priceRange;

        // Set new min/max keeping the center point stable
        Price newMaxPrice = priceAtCenter + relativePos * newPriceRange;
        Price newMinPrice = newMaxPrice - newPriceRange;

        m_scaler.setPriceRangeRaw(newMinPrice, newMaxPrice);
    }

    refresh();
}

void ChartPanel::onMouseLeave() {
    m_mouseInWindow = false;
    m_trackingMouse = false;
    refresh();
}

void ChartPanel::calculatePriceRange() {
    if (m_data.empty()) return;

    Price minPrice = m_data[0].low;
    Price maxPrice = m_data[0].high;

    for (const auto& bar : m_data) {
        minPrice = std::min(minPrice, bar.low);
        maxPrice = std::max(maxPrice, bar.high);
    }

    m_scaler.setPriceRange(minPrice, maxPrice);
}

void ChartPanel::drawBackground(HDC hdc, const RECT& rect) {
    HBRUSH bgBrush = CreateSolidBrush(Colors::ChartBackground);
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    // Draw axis separator lines
    int chartRight = m_scaler.getChartRight();
    int chartBottom = m_scaler.getChartBottom();
    int chartTop = m_scaler.getChartTop();
    int chartLeft = m_scaler.getChartLeft();

    HPEN axisPen = CreatePen(PS_SOLID, 1, Colors::AxisLine);
    HPEN oldPen = (HPEN)SelectObject(hdc, axisPen);

    // Right edge (separates chart from price axis)
    MoveToEx(hdc, chartRight, chartTop, NULL);
    LineTo(hdc, chartRight, chartBottom);

    // Bottom edge (separates chart from time axis)
    MoveToEx(hdc, chartLeft, chartBottom, NULL);
    LineTo(hdc, chartRight, chartBottom);

    SelectObject(hdc, oldPen);
    DeleteObject(axisPen);
}

void ChartPanel::drawCrosshair(HDC hdc) {
    int chartLeft = m_scaler.getChartLeft();
    int chartRight = m_scaler.getChartRight();
    int chartTop = m_scaler.getChartTop();
    int chartBottom = m_scaler.getChartBottom();

    // Only draw if mouse is in chart area
    if (m_mouseX < chartLeft || m_mouseX > chartRight ||
        m_mouseY < chartTop || m_mouseY > chartBottom) {
        return;
    }

    HPEN crosshairPen = CreatePen(PS_DOT, 1, Colors::Crosshair);
    HPEN oldPen = (HPEN)SelectObject(hdc, crosshairPen);

    // Horizontal line
    MoveToEx(hdc, chartLeft, m_mouseY, NULL);
    LineTo(hdc, chartRight, m_mouseY);

    // Vertical line
    MoveToEx(hdc, m_mouseX, chartTop, NULL);
    LineTo(hdc, m_mouseX, chartBottom);

    SelectObject(hdc, oldPen);
    DeleteObject(crosshairPen);

    HFONT font = CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, Colors::TextPrimary);
    SetBkMode(hdc, TRANSPARENT);

    // Draw price label on right side
    Price price = m_scaler.yToPrice(m_mouseY);
    char priceLabel[32];
    sprintf_s(priceLabel, "%.2f", price);

    SIZE textSize;
    GetTextExtentPoint32A(hdc, priceLabel, static_cast<int>(strlen(priceLabel)), &textSize);
    RECT labelRect = { chartRight + 2, m_mouseY - textSize.cy / 2 - 2,
                       chartRight + textSize.cx + 8, m_mouseY + textSize.cy / 2 + 2 };
    HBRUSH labelBrush = CreateSolidBrush(Colors::CrosshairLabel);
    FillRect(hdc, &labelRect, labelBrush);
    DeleteObject(labelBrush);

    TextOutA(hdc, chartRight + 5, m_mouseY - textSize.cy / 2, priceLabel, static_cast<int>(strlen(priceLabel)));

    // Draw time label at the bottom
    int barCount = m_scaler.getBarCount();
    int chartWidth = m_scaler.getChartWidth();
    if (barCount > 0 && chartWidth > 0 && !m_data.empty()) {
        double pixelsPerBar = static_cast<double>(chartWidth) / barCount;
        int scrollOffset = m_scaler.getScrollOffset();
        int adjustedX = m_mouseX - chartLeft - scrollOffset;
        int barIndex = static_cast<int>(adjustedX / pixelsPerBar);

        if (barIndex >= 0 && barIndex < static_cast<int>(m_data.size())) {
            const OHLCVBar& bar = m_data[barIndex];
            std::string timeLabel = DateTime::formatTimeShort(bar.timestamp);

            SIZE timeLabelSize;
            GetTextExtentPoint32A(hdc, timeLabel.c_str(), static_cast<int>(timeLabel.length()), &timeLabelSize);

            int timeLabelX = m_mouseX - timeLabelSize.cx / 2;
            int timeLabelY = chartBottom + 4;

            // Clamp to chart bounds
            if (timeLabelX < chartLeft) timeLabelX = chartLeft;
            if (timeLabelX + timeLabelSize.cx > chartRight) timeLabelX = chartRight - timeLabelSize.cx;

            RECT timeLabelRect = { timeLabelX - 3, timeLabelY - 1,
                                   timeLabelX + timeLabelSize.cx + 3, timeLabelY + timeLabelSize.cy + 1 };
            HBRUSH timeLabelBrush = CreateSolidBrush(Colors::CrosshairLabel);
            FillRect(hdc, &timeLabelRect, timeLabelBrush);
            DeleteObject(timeLabelBrush);

            TextOutA(hdc, timeLabelX, timeLabelY, timeLabel.c_str(), static_cast<int>(timeLabel.length()));
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void ChartPanel::drawInfoBox(HDC hdc) {
    if (m_data.empty()) return;

    int chartLeft = m_scaler.getChartLeft();
    int scrollOffset = m_scaler.getScrollOffset();
    int barCount = m_scaler.getBarCount();

    // Early exit if scaler is not properly initialized
    if (barCount <= 0) return;

    double chartWidth = static_cast<double>(m_scaler.getChartWidth());
    if (chartWidth <= 0) return;

    double pixelsPerBar = chartWidth / barCount;

    // Early exit if mouse is outside chart area horizontally
    int adjustedX = m_mouseX - chartLeft - scrollOffset;
    if (adjustedX < 0) return;

    int barIndex = static_cast<int>(adjustedX / pixelsPerBar);
    if (barIndex < 0 || barIndex >= static_cast<int>(m_data.size())) return;

    const OHLCVBar& bar = m_data[barIndex];

    // Format info text
    char info[256];
    sprintf_s(info, "O: %.2f  H: %.2f  L: %.2f  C: %.2f  V: %llu",
              bar.open, bar.high, bar.low, bar.close, bar.volume);

    std::string dateStr = DateTime::formatDate(bar.timestamp);

    // Draw info at top left
    HFONT font = CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, Colors::TextPrimary);
    SetBkMode(hdc, TRANSPARENT);

    TextOutA(hdc, 15, 15, dateStr.c_str(), static_cast<int>(dateStr.length()));
    TextOutA(hdc, 15, 30, info, static_cast<int>(strlen(info)));

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

void ChartPanel::onLeftButtonDown(int x, int y) {
    m_isDragging = true;
    m_dragStartX = x;
    m_dragStartY = y;
    m_scrollOffsetAtDragStart = m_scaler.getScrollOffset();
    m_minPriceAtLeftDragStart = m_scaler.getMinPrice();
    m_maxPriceAtLeftDragStart = m_scaler.getMaxPrice();
    SetCapture(m_hwnd);
}

void ChartPanel::onLeftButtonUp(int x, int y) {
    if (m_isDragging) {
        m_isDragging = false;
        ReleaseCapture();
    }
}

void ChartPanel::onLeftButtonDoubleClick(int x, int y) {
    fitToData();
}

void ChartPanel::onRightButtonDown(int x, int y) {
    m_isRightDragging = true;
    m_rightDragStartX = x;
    m_rightDragStartY = y;
    m_rightDragCenterX = x;  // Zoom will be centered on this point
    m_rightDragCenterY = y;
    m_barCountAtDragStart = m_scaler.getBarCount();
    m_minPriceAtDragStart = m_scaler.getMinPrice();
    m_maxPriceAtDragStart = m_scaler.getMaxPrice();
    m_scrollOffsetAtRightDragStart = m_scaler.getScrollOffset();
    SetCapture(m_hwnd);
}

void ChartPanel::onRightButtonUp(int x, int y) {
    if (m_isRightDragging) {
        m_isRightDragging = false;
        ReleaseCapture();
    }
}

} // namespace Origin
