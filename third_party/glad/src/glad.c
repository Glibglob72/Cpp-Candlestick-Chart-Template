#include <glad/glad.h>
#include <windows.h>

static HMODULE libGL = NULL;

static int open_gl(void) {
    libGL = LoadLibraryW(L"opengl32.dll");
    return libGL != NULL;
}

static void close_gl(void) {
    if (libGL) {
        FreeLibrary(libGL);
        libGL = NULL;
    }
}

static void* get_proc(const char *name) {
    void *result = (void*)wglGetProcAddress(name);
    if (!result || result == (void*)0x1 || result == (void*)0x2 ||
        result == (void*)0x3 || result == (void*)-1) {
        result = (void*)GetProcAddress(libGL, name);
    }
    return result;
}

// Function pointer definitions
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;

PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;

PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor = NULL;

PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;

PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;

PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;

PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced = NULL;

static void load_functions(void) {
    glad_glClear = (PFNGLCLEARPROC)get_proc("glClear");
    glad_glClearColor = (PFNGLCLEARCOLORPROC)get_proc("glClearColor");
    glad_glViewport = (PFNGLVIEWPORTPROC)get_proc("glViewport");
    glad_glEnable = (PFNGLENABLEPROC)get_proc("glEnable");
    glad_glDisable = (PFNGLDISABLEPROC)get_proc("glDisable");
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC)get_proc("glBlendFunc");
    glad_glGetError = (PFNGLGETERRORPROC)get_proc("glGetError");
    glad_glGetString = (PFNGLGETSTRINGPROC)get_proc("glGetString");
    glad_glFlush = (PFNGLFLUSHPROC)get_proc("glFlush");
    glad_glFinish = (PFNGLFINISHPROC)get_proc("glFinish");
    glad_glReadPixels = (PFNGLREADPIXELSPROC)get_proc("glReadPixels");

    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)get_proc("glGenBuffers");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)get_proc("glDeleteBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)get_proc("glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC)get_proc("glBufferData");
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)get_proc("glBufferSubData");

    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)get_proc("glGenVertexArrays");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)get_proc("glDeleteVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)get_proc("glBindVertexArray");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)get_proc("glEnableVertexAttribArray");
    glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)get_proc("glDisableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)get_proc("glVertexAttribPointer");
    glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)get_proc("glVertexAttribDivisor");

    glad_glCreateShader = (PFNGLCREATESHADERPROC)get_proc("glCreateShader");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)get_proc("glDeleteShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)get_proc("glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)get_proc("glCompileShader");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)get_proc("glGetShaderiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)get_proc("glGetShaderInfoLog");

    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)get_proc("glCreateProgram");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)get_proc("glDeleteProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)get_proc("glAttachShader");
    glad_glDetachShader = (PFNGLDETACHSHADERPROC)get_proc("glDetachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)get_proc("glLinkProgram");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)get_proc("glUseProgram");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)get_proc("glGetProgramiv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)get_proc("glGetProgramInfoLog");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)get_proc("glGetUniformLocation");

    glad_glUniform1f = (PFNGLUNIFORM1FPROC)get_proc("glUniform1f");
    glad_glUniform2f = (PFNGLUNIFORM2FPROC)get_proc("glUniform2f");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)get_proc("glUniform3f");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC)get_proc("glUniform4f");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)get_proc("glUniform1i");

    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)get_proc("glDrawArrays");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)get_proc("glDrawElements");
    glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)get_proc("glDrawArraysInstanced");
    glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)get_proc("glDrawElementsInstanced");
}

int gladLoadGL(void) {
    if (!open_gl()) return 0;
    load_functions();
    return 1;
}

int gladLoadGLLoader(GLADloadproc load) {
    (void)load;
    return gladLoadGL();
}
