#include "GLContext.h"
#include <glad/glad.h>

namespace Origin {

GLContext::~GLContext() {
    shutdown();
}

bool GLContext::initialize(HWND hwnd) {
    m_hwnd = hwnd;
    m_hdc = GetDC(hwnd);
    if (!m_hdc) return false;

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
    if (!pixelFormat || !SetPixelFormat(m_hdc, pixelFormat, &pfd)) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
        return false;
    }

    m_hglrc = wglCreateContext(m_hdc);
    if (!m_hglrc) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
        return false;
    }

    wglMakeCurrent(m_hdc, m_hglrc);

    if (!gladLoadGL()) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hglrc);
        ReleaseDC(m_hwnd, m_hdc);
        m_hglrc = nullptr;
        m_hdc = nullptr;
        return false;
    }

    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;
    glViewport(0, 0, m_width, m_height);

    return true;
}

void GLContext::shutdown() {
    if (m_hglrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hglrc);
        m_hglrc = nullptr;
    }
    if (m_hdc && m_hwnd) {
        ReleaseDC(m_hwnd, m_hdc);
        m_hdc = nullptr;
    }
}

void GLContext::makeCurrent() {
    if (m_hdc && m_hglrc) {
        wglMakeCurrent(m_hdc, m_hglrc);
    }
}

void GLContext::swapBuffers() {
    if (m_hdc) {
        SwapBuffers(m_hdc);
    }
}

void GLContext::resize(int width, int height) {
    m_width = width;
    m_height = height;
    if (m_hglrc) {
        makeCurrent();
        glViewport(0, 0, width, height);
    }
}

} // namespace Origin
