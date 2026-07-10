#pragma once

#include <windows.h>
#include <functional>
#include <string>
#include <vector>

namespace Origin {
    class ControlPanel {
    public:
        using FileSelectedCallback = std::function<void(const std::string&)>;
        using DaysChangedCallback = std::function<void(int)>;
        using FetchDataCallback = std::function<void()>;
        using SessionLinesChangedCallback = std::function<void(bool)>;

        static void registerClass(HINSTANCE hInstance);
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        ControlPanel();
        ~ControlPanel();

        // Create the control panel window
        bool create(HWND parent, int x, int y, int width, int height);

        // Set the list of available files
        void setFileList(const std::vector<std::string>& files);

        // Select a file in the combo box
        void selectFile(const std::string& filename);

        // Set the number of days
        void setDays(int days);

        // Set the maximum days limit (based on available data)
        void setMaxDays(int maxDays);

        // Set callbacks
        void setOnFileSelected(FileSelectedCallback callback) { m_fileCallback = callback; }
        void setOnDaysChanged(DaysChangedCallback callback) { m_daysCallback = callback; }
        void setOnFetchData(FetchDataCallback callback) { m_fetchCallback = callback; }
        void setOnSessionLinesChanged(SessionLinesChangedCallback callback) { m_sessionLinesCallback = callback; }

        // Get session lines checkbox state
        bool getSessionLinesEnabled() const { return m_sessionLinesEnabled; }

        // Get current days value
        int getDays() const { return m_daysValue; }

        // Update FPS counter (call this each frame from the chart panel)
        void notifyFrameRendered();

        // Get window handle and dimensions
        HWND getHandle() const { return m_hwnd; }
        static constexpr int getHeight() { return 56; }

    private:
        void onCreate();
        void onPaint();
        void onCommand(WPARAM wParam, LPARAM lParam);
        void onMouseMove(int x, int y);
        void onLButtonDown(int x, int y);
        void onLButtonUp(int x, int y);
        void createControls();
        void drawLabel(HDC hdc, const wchar_t* text, int x, int y);
        void drawDaysSpinner(HDC hdc);
        void drawArrow(HDC hdc, int cx, int cy, bool up, bool hovered, bool pressed);
        void drawSessionLinesCheckbox(DRAWITEMSTRUCT* dis);
        void updateDaysFromEdit();
        void updateEditFromDays();

        HWND m_hwnd = nullptr;
        HWND m_fileCombo = nullptr;
        HWND m_fetchButton = nullptr;
        HWND m_sessionLinesCheckbox = nullptr;
        bool m_sessionLinesEnabled = true;

        HFONT m_labelFont = nullptr;
        HFONT m_controlFont = nullptr;
        HFONT m_fpsFont = nullptr;

        std::vector<std::string> m_files;
        FileSelectedCallback m_fileCallback;
        DaysChangedCallback m_daysCallback;
        FetchDataCallback m_fetchCallback;
        SessionLinesChangedCallback m_sessionLinesCallback;

        // Days spinner state
        int m_daysValue = 5;
        int m_maxDays = 9999;
        HWND m_daysEdit = nullptr;
        RECT m_daysRect = {};
        RECT m_upArrowRect = {};
        RECT m_downArrowRect = {};
        bool m_upHovered = false;
        bool m_downHovered = false;
        bool m_upPressed = false;
        bool m_downPressed = false;

        // Control IDs
        static constexpr int ID_FILE_COMBO = 1001;
        static constexpr int ID_DAYS_EDIT = 1002;
        static constexpr int ID_FETCH_BUTTON = 1004;
        static constexpr int ID_SESSION_LINES_CHECKBOX = 1005;

        // FPS counter state
        LARGE_INTEGER m_lastFrameTime = {};
        LARGE_INTEGER m_frequency = {};
        int m_frameCount = 0;
        double m_fps = 0.0;
        RECT m_fpsRect = {};

        // Class name
        static constexpr const wchar_t* CLASS_NAME = L"OriginControlPanel";
    };
}
