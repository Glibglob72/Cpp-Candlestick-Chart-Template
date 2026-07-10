#include "MainWindow.h"
#include "../assets/Colors.h"
#include <algorithm>
#include <filesystem>

namespace Origin {

void MainWindow::registerClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(Colors::Background);
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.cbWndExtra = sizeof(MainWindow*);

    RegisterClassExW(&wc);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* window = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, 0));

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(window));
            window->m_hwnd = hwnd;
            window->onCreate();
            return 0;
        }

        case WM_DESTROY:
            if (window) window->onDestroy();
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            if (window) window->onSize(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            mmi->ptMinTrackSize.x = 800;
            mmi->ptMinTrackSize.y = 600;
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

MainWindow::MainWindow(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_dataRepository(std::make_unique<DataRepository>())
{
    // Find data folder relative to executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();

    // Try different possible data folder locations
    std::vector<std::filesystem::path> possiblePaths = {
        exeDir / "data",
        exeDir / ".." / "data",
        exeDir / ".." / ".." / "data",
        std::filesystem::current_path() / "data"
    };

    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            m_dataFolder = path.string();
            break;
        }
    }
}

MainWindow::~MainWindow() {}

bool MainWindow::create(int width, int height) {
    // Register child window classes
    ChartPanel::registerClass(m_hInstance);
    ControlPanel::registerClass(m_hInstance);

    m_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL,
        NULL,
        m_hInstance,
        this
    );

    if (m_hwnd) {
        // Enable dark mode title bar (Windows 10 1809+ / Windows 11)
        BOOL useDarkMode = TRUE;
        DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
    }

    return m_hwnd != nullptr;
}

void MainWindow::show(int nCmdShow) {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

int MainWindow::messageLoop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

void MainWindow::onCreate() {
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    int controlPanelHeight = ControlPanel::getHeight();

    // Create control panel at top
    m_controlPanel = std::make_unique<ControlPanel>();
    m_controlPanel->create(m_hwnd, 0, 0, width, controlPanelHeight);

    // Create chart panel below control panel
    m_chartPanel = std::make_unique<ChartPanel>();
    m_chartPanel->create(m_hwnd, 0, controlPanelHeight, width, height - controlPanelHeight);

    // Set up callbacks
    m_controlPanel->setOnFileSelected([this](const std::string& file) {
        onFileSelected(file);
    });

    m_controlPanel->setOnFetchData([this]() {
        onFetchData();
    });

    m_controlPanel->setOnSessionLinesChanged([this](bool enabled) {
        if (m_chartPanel) {
            m_chartPanel->setSessionLinesEnabled(enabled);
        }
    });

    // Set up FPS counter callback
    m_chartPanel->setOnFrameRendered([this]() {
        if (m_controlPanel) {
            m_controlPanel->notifyFrameRendered();
        }
    });

    // Scan for data files and populate combo box
    scanDataFolder();

    // Set initial file selection (but don't load data until user clicks Fetch)
    if (!m_dataFiles.empty()) {
        m_currentFile = m_dataFiles[0];
    }
}

void MainWindow::onDestroy() {
    m_chartPanel.reset();
    m_controlPanel.reset();
}

void MainWindow::onSize(int width, int height) {
    int controlPanelHeight = ControlPanel::getHeight();

    if (m_controlPanel && m_controlPanel->getHandle()) {
        SetWindowPos(m_controlPanel->getHandle(), NULL,
                     0, 0, width, controlPanelHeight,
                     SWP_NOZORDER);
    }

    if (m_chartPanel && m_chartPanel->getHandle()) {
        SetWindowPos(m_chartPanel->getHandle(), NULL,
                     0, controlPanelHeight, width, height - controlPanelHeight,
                     SWP_NOZORDER);
    }
}

void MainWindow::onFileSelected(const std::string& filename) {
    m_currentFile = filename;
}

void MainWindow::onFetchData() {
    if (m_controlPanel) {
        m_numDays = m_controlPanel->getDays();
    }
    loadData();
}

void MainWindow::scanDataFolder() {
    m_dataFiles.clear();

    if (m_dataFolder.empty()) return;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_dataFolder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".csv") {
                    m_dataFiles.push_back(entry.path().filename().string());
                }
            }
        }
    }
    catch (...) {
        // Failed to scan directory
    }

    // Sort files alphabetically
    std::sort(m_dataFiles.begin(), m_dataFiles.end());

    // Update control panel
    if (m_controlPanel) {
        m_controlPanel->setFileList(m_dataFiles);
    }
}

void MainWindow::loadData() {
    if (m_currentFile.empty() || m_dataFolder.empty()) return;

    std::string filepath = m_dataFolder + "\\" + m_currentFile;

    try {
        // Update window title to show loading
        SetWindowTextW(m_hwnd, L"OHLCV Viewer - Loading...");

        m_dataRepository->loadFile(filepath);

        // Update window title with file info
        int totalDays = m_dataRepository->getTotalDaysCount();
        std::wstring title = L"OHLCV Viewer - " +
            std::wstring(m_currentFile.begin(), m_currentFile.end()) +
            L" (" + std::to_wstring(totalDays) + L" trading days)";
        size_t skipped = m_dataRepository->getSkippedLineCount();
        if (skipped > 0) {
            title += L" - " + std::to_wstring(skipped) + L" malformed lines skipped";
        }
        SetWindowTextW(m_hwnd, title.c_str());

        // Update max days limit based on available data
        if (m_controlPanel) {
            m_controlPanel->setMaxDays(totalDays);
        }

        updateChart();
    }
    catch (const std::exception& e) {
        std::wstring errorMsg = L"Failed to load file: ";
        std::string what = e.what();
        errorMsg += std::wstring(what.begin(), what.end());
        MessageBoxW(m_hwnd, errorMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
    }
}

void MainWindow::updateChart() {
    if (!m_dataRepository->hasData() || !m_chartPanel) return;

    OHLCVData displayData = m_dataRepository->getBarsForDays(m_numDays);
    m_chartPanel->setData(displayData);
}

} // namespace Origin
