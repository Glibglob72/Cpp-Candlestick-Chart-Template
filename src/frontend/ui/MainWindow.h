#pragma once

#include "../chart/ChartPanel.h"
#include "ControlPanel.h"
#include "../../backend/DataRepository.h"
#include <windows.h>
#include <dwmapi.h>
#include <memory>
#include <string>
#include <vector>

namespace Origin {
    class MainWindow {
    public:
        static void registerClass(HINSTANCE hInstance);
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        MainWindow(HINSTANCE hInstance);
        ~MainWindow();

        // Create and show the window
        bool create(int width, int height);
        void show(int nCmdShow);

        // Run the message loop
        int messageLoop();

    private:
        void onCreate();
        void onDestroy();
        void onSize(int width, int height);
        void onFileSelected(const std::string& filename);
        void onFetchData();

        void scanDataFolder();
        void loadData();
        void updateChart();

        HINSTANCE m_hInstance;
        HWND m_hwnd = nullptr;

        std::unique_ptr<ChartPanel> m_chartPanel;
        std::unique_ptr<ControlPanel> m_controlPanel;
        std::unique_ptr<DataRepository> m_dataRepository;

        std::vector<std::string> m_dataFiles;
        std::string m_currentFile;
        int m_numDays = 5;
        std::string m_dataFolder;

        // Class name
        static constexpr const wchar_t* CLASS_NAME = L"OriginMainWindow";
        static constexpr const wchar_t* WINDOW_TITLE = L"OHLCV Viewer";
    };
}
