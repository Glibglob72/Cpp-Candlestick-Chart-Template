#include "frontend/ui/MainWindow.h"
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

// Enable visual styles
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
    // Unused parameters
    (void)hPrevInstance;
    (void)lpCmdLine;

    // Initialize common controls (for up-down controls, etc.)
    INITCOMMONCONTROLSEX icc = {};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&icc);

    // Register main window class
    Origin::MainWindow::registerClass(hInstance);

    // Create and show main window
    Origin::MainWindow mainWindow(hInstance);
    if (!mainWindow.create(1280, 800)) {
        MessageBoxW(NULL, L"Failed to create main window", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    mainWindow.show(nCmdShow);

    // Run message loop
    return mainWindow.messageLoop();
}
