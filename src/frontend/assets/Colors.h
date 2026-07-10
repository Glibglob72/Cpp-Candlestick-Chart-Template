#pragma once

#include <windows.h>

namespace Origin::Colors {
    // Obsidian black background colors
    constexpr COLORREF Background = RGB(16, 16, 16);
    constexpr COLORREF PanelBackground = RGB(22, 22, 22);
    constexpr COLORREF ChartBackground = RGB(12, 12, 12);

    // Grid and axis colors
    constexpr COLORREF GridLine = RGB(32, 32, 32);
    constexpr COLORREF GridLineMajor = RGB(45, 45, 45);
    constexpr COLORREF AxisLine = RGB(55, 55, 55);

    // Text colors
    constexpr COLORREF TextPrimary = RGB(220, 220, 220);
    constexpr COLORREF TextSecondary = RGB(140, 140, 140);
    constexpr COLORREF TextMuted = RGB(90, 90, 90);

    // Candlestick colors
    constexpr COLORREF BullishBody = RGB(38, 166, 91);
    constexpr COLORREF BullishWick = RGB(38, 166, 91);
    constexpr COLORREF BearishBody = RGB(214, 69, 65);
    constexpr COLORREF BearishWick = RGB(214, 69, 65);

    // UI element colors
    constexpr COLORREF ButtonNormal = RGB(35, 35, 35);
    constexpr COLORREF ButtonHover = RGB(50, 50, 50);
    constexpr COLORREF ButtonPressed = RGB(28, 28, 28);
    constexpr COLORREF Accent = RGB(180, 180, 180);
    constexpr COLORREF Border = RGB(40, 40, 40);

    // Crosshair
    constexpr COLORREF Crosshair = RGB(100, 100, 100);
    constexpr COLORREF CrosshairLabel = RGB(60, 60, 60);

    // Markers
    constexpr COLORREF MarkerLine = RGB(80, 80, 80);
}
