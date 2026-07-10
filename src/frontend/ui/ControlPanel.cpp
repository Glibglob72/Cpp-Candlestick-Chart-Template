#include "ControlPanel.h"
#include "../assets/Colors.h"
#include <commctrl.h>
#include <uxtheme.h>
#include <windowsx.h>

#pragma comment(lib, "uxtheme.lib")

namespace Origin {

void ControlPanel::registerClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ControlPanel::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(Colors::PanelBackground);
    wc.lpszClassName = CLASS_NAME;
    wc.cbWndExtra = sizeof(ControlPanel*);

    RegisterClassExW(&wc);
}

LRESULT CALLBACK ControlPanel::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ControlPanel* panel = reinterpret_cast<ControlPanel*>(GetWindowLongPtr(hwnd, 0));

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            panel = reinterpret_cast<ControlPanel*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(panel));
            panel->m_hwnd = hwnd;
            panel->onCreate();
            return 0;
        }

        case WM_PAINT:
            if (panel) panel->onPaint();
            return 0;

        case WM_COMMAND:
            if (panel) panel->onCommand(wParam, lParam);
            return 0;

        case WM_MOUSEMOVE:
            if (panel) panel->onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_LBUTTONDOWN:
            if (panel) panel->onLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_LBUTTONUP:
            if (panel) panel->onLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;

        case WM_MOUSELEAVE:
            if (panel) {
                panel->m_upHovered = false;
                panel->m_downHovered = false;
                InvalidateRect(hwnd, &panel->m_daysRect, FALSE);
            }
            return 0;

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            HDC hdcControl = (HDC)wParam;
            SetTextColor(hdcControl, Colors::TextPrimary);
            SetBkColor(hdcControl, Colors::ButtonNormal);
            static HBRUSH hEditBrush = CreateSolidBrush(Colors::ButtonNormal);
            return (LRESULT)hEditBrush;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdcControl = (HDC)wParam;
            SetTextColor(hdcControl, Colors::TextSecondary);
            SetBkMode(hdcControl, TRANSPARENT);
            static HBRUSH hStaticBrush = CreateSolidBrush(Colors::PanelBackground);
            return (LRESULT)hStaticBrush;
        }

        case WM_DRAWITEM:
            if (panel) {
                DRAWITEMSTRUCT* dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
                if (dis->CtlID == ID_SESSION_LINES_CHECKBOX) {
                    panel->drawSessionLinesCheckbox(dis);
                    return TRUE;
                }
            }
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

ControlPanel::ControlPanel() {
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_lastFrameTime);
}

ControlPanel::~ControlPanel() {
    if (m_labelFont) DeleteObject(m_labelFont);
    if (m_controlFont) DeleteObject(m_controlFont);
    if (m_fpsFont) DeleteObject(m_fpsFont);
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool ControlPanel::create(HWND parent, int x, int y, int width, int height) {
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

void ControlPanel::onCreate() {
    // Create fonts
    m_labelFont = CreateFontW(11, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                              ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    m_controlFont = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    m_fpsFont = CreateFontW(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    createControls();
}

void ControlPanel::createControls() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Layout constants
    int leftMargin = 20;
    int controlY = 20;
    int controlHeight = 22;
    int spacing = 24;

    int xPos = leftMargin;

    // Symbol combo box
    m_fileCombo = CreateWindowExW(
        0, L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        xPos, controlY, 160, 200,
        m_hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_FILE_COMBO)), hInstance, NULL
    );
    SetWindowTheme(m_fileCombo, L"DarkMode_CFD", NULL);
    xPos += 160 + spacing;

    // Custom days spinner - match combo box height and style
    int spinnerWidth = 60;
    int arrowWidth = 16;
    m_daysRect = { xPos, controlY, xPos + spinnerWidth, controlY + controlHeight };

    // Create edit control for the number input (excluding arrow area)
    // Vertically center the edit control (font is ~13px, control is 22px, so offset by ~3px)
    int editHeight = 16;
    int editY = controlY + (controlHeight - editHeight) / 2;
    m_daysEdit = CreateWindowExW(
        0, L"EDIT", L"5",
        WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
        xPos + 2, editY, spinnerWidth - arrowWidth - 4, editHeight,
        m_hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_DAYS_EDIT)), hInstance, NULL
    );
    SendMessage(m_daysEdit, EM_SETLIMITTEXT, 4, 0);  // Max 4 digits

    // Arrow hit areas (stacked vertically on the right side)
    int arrowHeight = controlHeight / 2;
    m_upArrowRect = { xPos + spinnerWidth - arrowWidth, m_daysRect.top,
                      xPos + spinnerWidth, m_daysRect.top + arrowHeight };
    m_downArrowRect = { xPos + spinnerWidth - arrowWidth, m_daysRect.top + arrowHeight,
                        xPos + spinnerWidth, m_daysRect.bottom };
    xPos += spinnerWidth + spacing;

    // Fetch button
    m_fetchButton = CreateWindowExW(
        0, L"BUTTON", L"Load",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        xPos, controlY, 70, controlHeight,
        m_hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_FETCH_BUTTON)), hInstance, NULL
    );
    SetWindowTheme(m_fetchButton, L"DarkMode_Explorer", NULL);
    xPos += 70 + spacing;

    // Session lines checkbox (owner-drawn for dark theme)
    m_sessionLinesCheckbox = CreateWindowExW(
        0, L"BUTTON", L"18:00",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        xPos, controlY, 60, controlHeight,
        m_hwnd, reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_SESSION_LINES_CHECKBOX)), hInstance, NULL
    );

    // Apply fonts
    SendMessage(m_fileCombo, WM_SETFONT, (WPARAM)m_controlFont, TRUE);
    SendMessage(m_daysEdit, WM_SETFONT, (WPARAM)m_controlFont, TRUE);
    SendMessage(m_fetchButton, WM_SETFONT, (WPARAM)m_controlFont, TRUE);
    SendMessage(m_sessionLinesCheckbox, WM_SETFONT, (WPARAM)m_controlFont, TRUE);
}

void ControlPanel::drawLabel(HDC hdc, const wchar_t* text, int x, int y) {
    HFONT oldFont = (HFONT)SelectObject(hdc, m_labelFont);
    SetTextColor(hdc, Colors::TextMuted);
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, x, y, text, static_cast<int>(wcslen(text)));
    SelectObject(hdc, oldFont);
}

void ControlPanel::onPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    RECT rect;
    GetClientRect(m_hwnd, &rect);

    // Fill background
    HBRUSH bgBrush = CreateSolidBrush(Colors::PanelBackground);
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    // Draw labels above controls
    int labelY = 5;
    drawLabel(hdc, L"SYMBOL", 20, labelY);
    drawLabel(hdc, L"DAYS", 20 + 160 + 24, labelY);

    // Draw the custom days spinner
    drawDaysSpinner(hdc);

    // Draw FPS counter on the right side
    wchar_t fpsText[32];
    swprintf_s(fpsText, L"%.1f FPS", m_fps);

    HFONT oldFont = (HFONT)SelectObject(hdc, m_fpsFont);
    SetTextColor(hdc, Colors::TextPrimary);
    SetBkMode(hdc, TRANSPARENT);

    SIZE textSize;
    GetTextExtentPoint32W(hdc, fpsText, static_cast<int>(wcslen(fpsText)), &textSize);
    int fpsX = rect.right - textSize.cx - 20;
    int fpsY = (rect.bottom - textSize.cy) / 2;
    TextOutW(hdc, fpsX, fpsY, fpsText, static_cast<int>(wcslen(fpsText)));

    // Store FPS rect for invalidation
    m_fpsRect = { fpsX - 5, fpsY - 2, rect.right - 15, fpsY + textSize.cy + 2 };

    SelectObject(hdc, oldFont);

    // Draw subtle bottom border
    HPEN borderPen = CreatePen(PS_SOLID, 1, Colors::Border);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    MoveToEx(hdc, 0, rect.bottom - 1, NULL);
    LineTo(hdc, rect.right, rect.bottom - 1);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);

    EndPaint(m_hwnd, &ps);
}

void ControlPanel::drawDaysSpinner(HDC hdc) {
    // Draw background - same as combo box
    HBRUSH bgBrush = CreateSolidBrush(Colors::ButtonNormal);
    FillRect(hdc, &m_daysRect, bgBrush);
    DeleteObject(bgBrush);

    // Draw border - match combo box style
    HPEN borderPen = CreatePen(PS_SOLID, 1, Colors::Accent);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, m_daysRect.left, m_daysRect.top, m_daysRect.right, m_daysRect.bottom);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(borderPen);

    // Draw up arrow
    int upCx = (m_upArrowRect.left + m_upArrowRect.right) / 2;
    int upCy = (m_upArrowRect.top + m_upArrowRect.bottom) / 2;
    drawArrow(hdc, upCx, upCy, true, m_upHovered, m_upPressed);

    // Draw down arrow
    int downCx = (m_downArrowRect.left + m_downArrowRect.right) / 2;
    int downCy = (m_downArrowRect.top + m_downArrowRect.bottom) / 2;
    drawArrow(hdc, downCx, downCy, false, m_downHovered, m_downPressed);
}

void ControlPanel::drawArrow(HDC hdc, int cx, int cy, bool up, bool hovered, bool pressed) {
    // Arrow dimensions - smaller to fit compact control
    int arrowWidth = 6;
    int arrowHeight = 3;

    // Determine color based on state
    COLORREF color;
    if (pressed) {
        color = Colors::TextPrimary;
    } else if (hovered) {
        color = Colors::TextSecondary;
    } else {
        color = Colors::TextMuted;
    }

    // Create pen and brush
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HBRUSH brush = CreateSolidBrush(color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    // Define triangle points
    POINT points[3];
    if (up) {
        points[0] = { cx, cy - arrowHeight / 2 };          // top
        points[1] = { cx - arrowWidth / 2, cy + arrowHeight / 2 };  // bottom left
        points[2] = { cx + arrowWidth / 2, cy + arrowHeight / 2 };  // bottom right
    } else {
        points[0] = { cx, cy + arrowHeight / 2 };          // bottom
        points[1] = { cx - arrowWidth / 2, cy - arrowHeight / 2 };  // top left
        points[2] = { cx + arrowWidth / 2, cy - arrowHeight / 2 };  // top right
    }

    Polygon(hdc, points, 3);

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

void ControlPanel::drawSessionLinesCheckbox(DRAWITEMSTRUCT* dis) {
    HDC hdc = dis->hDC;
    RECT rect = dis->rcItem;

    // Fill background
    HBRUSH bgBrush = CreateSolidBrush(Colors::PanelBackground);
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    // Checkbox box dimensions
    int boxSize = 14;
    int boxY = rect.top + (rect.bottom - rect.top - boxSize) / 2;
    RECT boxRect = { rect.left, boxY, rect.left + boxSize, boxY + boxSize };

    // Draw checkbox box background
    HBRUSH boxBgBrush = CreateSolidBrush(Colors::ButtonNormal);
    FillRect(hdc, &boxRect, boxBgBrush);
    DeleteObject(boxBgBrush);

    // Draw checkbox border
    HPEN borderPen = CreatePen(PS_SOLID, 1, Colors::Accent);
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, boxRect.left, boxRect.top, boxRect.right, boxRect.bottom);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(borderPen);

    // Draw checkmark if checked
    if (m_sessionLinesEnabled) {
        HPEN checkPen = CreatePen(PS_SOLID, 2, Colors::TextPrimary);
        oldPen = (HPEN)SelectObject(hdc, checkPen);

        // Draw a nice checkmark - short leg then long leg
        int left = boxRect.left + 3;
        int top = boxRect.top + 3;
        int bottom = boxRect.bottom - 3;
        int right = boxRect.right - 3;
        int midX = left + (right - left) / 3;
        int midY = bottom - 1;

        // Short downward stroke
        MoveToEx(hdc, left + 1, top + (bottom - top) / 2, NULL);
        LineTo(hdc, midX, midY);
        // Long upward stroke
        LineTo(hdc, right - 1, top + 1);

        SelectObject(hdc, oldPen);
        DeleteObject(checkPen);
    }

    // Draw label text
    HFONT oldFont = (HFONT)SelectObject(hdc, m_controlFont);
    SetTextColor(hdc, Colors::TextSecondary);
    SetBkMode(hdc, TRANSPARENT);

    RECT textRect = { rect.left + boxSize + 4, rect.top, rect.right, rect.bottom };
    DrawTextW(hdc, L"18:00", -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, oldFont);
}

void ControlPanel::onMouseMove(int x, int y) {
    POINT pt = { x, y };

    bool wasUpHovered = m_upHovered;
    bool wasDownHovered = m_downHovered;

    m_upHovered = PtInRect(&m_upArrowRect, pt);
    m_downHovered = PtInRect(&m_downArrowRect, pt);

    // Track mouse for WM_MOUSELEAVE
    if (m_upHovered || m_downHovered) {
        TRACKMOUSEEVENT tme = {};
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hwnd;
        TrackMouseEvent(&tme);
    }

    if (wasUpHovered != m_upHovered || wasDownHovered != m_downHovered) {
        InvalidateRect(m_hwnd, &m_daysRect, FALSE);
    }
}

void ControlPanel::onLButtonDown(int x, int y) {
    POINT pt = { x, y };

    if (PtInRect(&m_upArrowRect, pt)) {
        m_upPressed = true;
        if (m_daysValue < m_maxDays) {
            m_daysValue++;
            updateEditFromDays();
            if (m_daysCallback) m_daysCallback(m_daysValue);
        }
        InvalidateRect(m_hwnd, &m_daysRect, FALSE);
        SetCapture(m_hwnd);
    } else if (PtInRect(&m_downArrowRect, pt)) {
        m_downPressed = true;
        if (m_daysValue > 1) {
            m_daysValue--;
            updateEditFromDays();
            if (m_daysCallback) m_daysCallback(m_daysValue);
        }
        InvalidateRect(m_hwnd, &m_daysRect, FALSE);
        SetCapture(m_hwnd);
    }
}

void ControlPanel::onLButtonUp(int x, int y) {
    (void)x; (void)y;
    m_upPressed = false;
    m_downPressed = false;
    ReleaseCapture();
    InvalidateRect(m_hwnd, &m_daysRect, FALSE);
}

void ControlPanel::onCommand(WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    int controlId = LOWORD(wParam);
    int notifyCode = HIWORD(wParam);

    if (controlId == ID_FILE_COMBO && notifyCode == CBN_SELCHANGE) {
        int index = static_cast<int>(SendMessage(m_fileCombo, CB_GETCURSEL, 0, 0));
        if (index >= 0 && index < static_cast<int>(m_files.size())) {
            if (m_fileCallback) {
                m_fileCallback(m_files[index]);
            }
        }
    }
    else if (controlId == ID_DAYS_EDIT && notifyCode == EN_KILLFOCUS) {
        // When edit loses focus, validate and update the value
        updateDaysFromEdit();
    }
    else if (controlId == ID_FETCH_BUTTON && notifyCode == BN_CLICKED) {
        if (m_fetchCallback) {
            m_fetchCallback();
        }
    }
    else if (controlId == ID_SESSION_LINES_CHECKBOX && notifyCode == BN_CLICKED) {
        // Toggle the state (owner-drawn checkbox doesn't auto-toggle)
        m_sessionLinesEnabled = !m_sessionLinesEnabled;
        InvalidateRect(m_sessionLinesCheckbox, NULL, FALSE);
        if (m_sessionLinesCallback) {
            m_sessionLinesCallback(m_sessionLinesEnabled);
        }
    }
}

void ControlPanel::setFileList(const std::vector<std::string>& files) {
    m_files = files;

    SendMessage(m_fileCombo, CB_RESETCONTENT, 0, 0);

    for (const auto& file : files) {
        // Extract symbol name from filename (e.g., "MES_v0_ohlcv_1m.csv" -> "MES")
        std::string symbol = file;
        size_t underscorePos = file.find('_');
        if (underscorePos != std::string::npos) {
            symbol = file.substr(0, underscorePos);
        }

        int len = MultiByteToWideChar(CP_UTF8, 0, symbol.c_str(), -1, NULL, 0);
        std::wstring wsymbol(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, symbol.c_str(), -1, &wsymbol[0], len);

        SendMessageW(m_fileCombo, CB_ADDSTRING, 0, (LPARAM)wsymbol.c_str());
    }

    if (!files.empty()) {
        SendMessage(m_fileCombo, CB_SETCURSEL, 0, 0);
    }
}

void ControlPanel::selectFile(const std::string& filename) {
    for (size_t i = 0; i < m_files.size(); ++i) {
        if (m_files[i] == filename) {
            SendMessage(m_fileCombo, CB_SETCURSEL, i, 0);
            break;
        }
    }
}

void ControlPanel::setDays(int days) {
    if (days >= 1 && days <= m_maxDays) {
        m_daysValue = days;
        updateEditFromDays();
        InvalidateRect(m_hwnd, &m_daysRect, FALSE);
    }
}

void ControlPanel::setMaxDays(int maxDays) {
    m_maxDays = maxDays > 0 ? maxDays : 1;
    if (m_daysValue > m_maxDays) {
        m_daysValue = m_maxDays;
        updateEditFromDays();
        if (m_daysCallback) m_daysCallback(m_daysValue);
    }
}

void ControlPanel::updateDaysFromEdit() {
    wchar_t buffer[16];
    GetWindowTextW(m_daysEdit, buffer, 16);
    int value = _wtoi(buffer);

    // Clamp to valid range
    if (value < 1) value = 1;
    if (value > m_maxDays) value = m_maxDays;

    if (value != m_daysValue) {
        m_daysValue = value;
        if (m_daysCallback) m_daysCallback(m_daysValue);
    }

    // Update display to show clamped value
    updateEditFromDays();
}

void ControlPanel::updateEditFromDays() {
    wchar_t buffer[16];
    wsprintfW(buffer, L"%d", m_daysValue);
    SetWindowTextW(m_daysEdit, buffer);
}

void ControlPanel::notifyFrameRendered() {
    m_frameCount++;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    double elapsed = static_cast<double>(currentTime.QuadPart - m_lastFrameTime.QuadPart) /
                     static_cast<double>(m_frequency.QuadPart);

    // Update FPS every 100ms
    if (elapsed >= 0.1) {
        m_fps = m_frameCount / elapsed;
        m_frameCount = 0;
        m_lastFrameTime = currentTime;

        // Invalidate just the FPS area to trigger redraw
        if (m_hwnd) {
            InvalidateRect(m_hwnd, &m_fpsRect, FALSE);
        }
    }
}

} // namespace Origin
