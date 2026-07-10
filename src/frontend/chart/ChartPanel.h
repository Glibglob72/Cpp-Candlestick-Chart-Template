#pragma once

#include "../../common/OHLCVBar.h"
#include "ChartScaler.h"
#include "CandleRenderer.h"
#include "PriceAxis.h"
#include "TimeAxis.h"
#include "gl/GLContext.h"
#include "gl/GLCandleRenderer.h"
#include <windows.h>
#include <string>
#include <functional>

namespace Origin {
    class ChartPanel {
    public:
        static void registerClass(HINSTANCE hInstance);
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        ChartPanel();
        ~ChartPanel();

        // Create the chart panel window
        bool create(HWND parent, int x, int y, int width, int height);

        // Set the data to display
        void setData(const OHLCVData& data);

        // Refresh the chart
        void refresh();

        // Fit chart to show all data
        void fitToData();

        // Enable/disable 18:00 session lines
        void setSessionLinesEnabled(bool enabled);

        // Get the window handle
        HWND getHandle() const { return m_hwnd; }

        // Set callback for frame rendered notification (for FPS counter)
        using FrameRenderedCallback = std::function<void()>;
        void setOnFrameRendered(FrameRenderedCallback callback) { m_frameCallback = callback; }

    private:
        void onCreate();
        void onDestroy();
        void onPaint();
        void onSize(int width, int height);
        void onMouseMove(int x, int y);
        void onMouseLeave();
        void onLeftButtonDown(int x, int y);
        void onLeftButtonUp(int x, int y);
        void onLeftButtonDoubleClick(int x, int y);
        void onRightButtonDown(int x, int y);
        void onRightButtonUp(int x, int y);

        void calculatePriceRange();
        void drawBackground(HDC hdc, const RECT& rect);
        void drawCrosshair(HDC hdc);
        void drawInfoBox(HDC hdc);

        HWND m_hwnd = nullptr;
        OHLCVData m_data;

        // Rendering components
        ChartScaler m_scaler;
        CandleRenderer m_candleRenderer;
        PriceAxis m_priceAxis;
        TimeAxis m_timeAxis;

        // OpenGL rendering
        GLContext m_glContext;
        GLCandleRenderer m_glCandleRenderer;
        bool m_useOpenGL = true;

        // Double buffering
        HDC m_memDC = nullptr;
        HBITMAP m_memBitmap = nullptr;
        int m_bufferWidth = 0;
        int m_bufferHeight = 0;

        // Mouse state
        int m_mouseX = -1;
        int m_mouseY = -1;
        bool m_mouseInWindow = false;
        bool m_trackingMouse = false;

        // Left-click dragging state (2D panning)
        bool m_isDragging = false;
        int m_dragStartX = 0;
        int m_dragStartY = 0;
        int m_scrollOffsetAtDragStart = 0;
        Price m_minPriceAtLeftDragStart = 0;
        Price m_maxPriceAtLeftDragStart = 0;

        // Right-click dragging state (zooming)
        bool m_isRightDragging = false;
        int m_rightDragStartX = 0;
        int m_rightDragStartY = 0;
        int m_rightDragCenterX = 0;  // Center point for zoom (cursor position at drag start)
        int m_rightDragCenterY = 0;
        int m_barCountAtDragStart = 0;
        Price m_minPriceAtDragStart = 0;
        Price m_maxPriceAtDragStart = 0;
        int m_scrollOffsetAtRightDragStart = 0;

        // Frame rendered callback
        FrameRenderedCallback m_frameCallback;

        // Class name
        static constexpr const wchar_t* CLASS_NAME = L"OriginChartPanel";
    };
}
