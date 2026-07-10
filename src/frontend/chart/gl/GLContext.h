#pragma once

#include <windows.h>

namespace Origin {

class GLContext {
public:
    GLContext() = default;
    ~GLContext();

    bool initialize(HWND hwnd);
    void shutdown();
    void makeCurrent();
    void swapBuffers();
    void resize(int width, int height);

    bool isValid() const { return m_hglrc != nullptr; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    HGLRC m_hglrc = nullptr;
    int m_width = 0;
    int m_height = 0;
};

} // namespace Origin
