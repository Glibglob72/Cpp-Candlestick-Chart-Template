#ifndef __glad_h_
#define __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif
#define __gl_h_

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (* GLADloadproc)(const char *name);

GLAPI int gladLoadGL(void);
GLAPI int gladLoadGLLoader(GLADloadproc);

#include <KHR/khrplatform.h>
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_int16_t GLshort;
typedef khronos_uint16_t GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef khronos_int32_t GLclampx;
typedef int GLsizei;
typedef khronos_float_t GLfloat;
typedef khronos_float_t GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglClientBufferEXT;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
#ifdef __APPLE__
typedef void *GLhandleARB;
#else
typedef unsigned int GLhandleARB;
#endif
typedef khronos_uint16_t GLhalf;
typedef khronos_uint16_t GLhalfARB;
typedef khronos_int32_t GLfixed;
typedef khronos_intptr_t GLintptr;
typedef khronos_intptr_t GLintptrARB;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_ssize_t GLsizeiptrARB;
typedef khronos_int64_t GLint64;
typedef khronos_int64_t GLint64EXT;
typedef khronos_uint64_t GLuint64;
typedef khronos_uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;
typedef void (APIENTRY *GLVULKANPROCNV)(void);

#define GL_FALSE 0
#define GL_TRUE 1
#define GL_NONE 0
#define GL_NO_ERROR 0
#define GL_ZERO 0
#define GL_ONE 1

// Buffer/Array constants
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8

// Draw modes
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006

// Clear bits
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000

// Data types
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_DOUBLE 0x140A

// Shader types
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

// Shader params
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

// Blend
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

// Enable caps
#define GL_LINE_SMOOTH 0x0B20
#define GL_DEPTH_TEST 0x0B71

// Pixel formats
#define GL_BGRA_EXT 0x80E1
#define GL_RGBA 0x1908
#define GL_RGB 0x1907

// Function pointers
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)(void);
typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
typedef void (APIENTRYP PFNGLFLUSHPROC)(void);
typedef void (APIENTRYP PFNGLFINISHPROC)(void);
typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);

// Buffer functions
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

// VAO functions
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);

// Shader functions
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

// Program functions
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);

// Uniform functions
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);

// Draw functions
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);

// Function declarations
GLAPI PFNGLCLEARPROC glad_glClear;
#define glClear glad_glClear
GLAPI PFNGLCLEARCOLORPROC glad_glClearColor;
#define glClearColor glad_glClearColor
GLAPI PFNGLVIEWPORTPROC glad_glViewport;
#define glViewport glad_glViewport
GLAPI PFNGLENABLEPROC glad_glEnable;
#define glEnable glad_glEnable
GLAPI PFNGLDISABLEPROC glad_glDisable;
#define glDisable glad_glDisable
GLAPI PFNGLBLENDFUNCPROC glad_glBlendFunc;
#define glBlendFunc glad_glBlendFunc
GLAPI PFNGLGETERRORPROC glad_glGetError;
#define glGetError glad_glGetError
GLAPI PFNGLGETSTRINGPROC glad_glGetString;
#define glGetString glad_glGetString
GLAPI PFNGLFLUSHPROC glad_glFlush;
#define glFlush glad_glFlush
GLAPI PFNGLFINISHPROC glad_glFinish;
#define glFinish glad_glFinish
GLAPI PFNGLREADPIXELSPROC glad_glReadPixels;
#define glReadPixels glad_glReadPixels

GLAPI PFNGLGENBUFFERSPROC glad_glGenBuffers;
#define glGenBuffers glad_glGenBuffers
GLAPI PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
#define glDeleteBuffers glad_glDeleteBuffers
GLAPI PFNGLBINDBUFFERPROC glad_glBindBuffer;
#define glBindBuffer glad_glBindBuffer
GLAPI PFNGLBUFFERDATAPROC glad_glBufferData;
#define glBufferData glad_glBufferData
GLAPI PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
#define glBufferSubData glad_glBufferSubData

GLAPI PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
#define glGenVertexArrays glad_glGenVertexArrays
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
#define glDeleteVertexArrays glad_glDeleteVertexArrays
GLAPI PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
#define glBindVertexArray glad_glBindVertexArray
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
#define glEnableVertexAttribArray glad_glEnableVertexAttribArray
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
#define glDisableVertexAttribArray glad_glDisableVertexAttribArray
GLAPI PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#define glVertexAttribPointer glad_glVertexAttribPointer
GLAPI PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor;
#define glVertexAttribDivisor glad_glVertexAttribDivisor

GLAPI PFNGLCREATESHADERPROC glad_glCreateShader;
#define glCreateShader glad_glCreateShader
GLAPI PFNGLDELETESHADERPROC glad_glDeleteShader;
#define glDeleteShader glad_glDeleteShader
GLAPI PFNGLSHADERSOURCEPROC glad_glShaderSource;
#define glShaderSource glad_glShaderSource
GLAPI PFNGLCOMPILESHADERPROC glad_glCompileShader;
#define glCompileShader glad_glCompileShader
GLAPI PFNGLGETSHADERIVPROC glad_glGetShaderiv;
#define glGetShaderiv glad_glGetShaderiv
GLAPI PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
#define glGetShaderInfoLog glad_glGetShaderInfoLog

GLAPI PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
#define glCreateProgram glad_glCreateProgram
GLAPI PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
#define glDeleteProgram glad_glDeleteProgram
GLAPI PFNGLATTACHSHADERPROC glad_glAttachShader;
#define glAttachShader glad_glAttachShader
GLAPI PFNGLDETACHSHADERPROC glad_glDetachShader;
#define glDetachShader glad_glDetachShader
GLAPI PFNGLLINKPROGRAMPROC glad_glLinkProgram;
#define glLinkProgram glad_glLinkProgram
GLAPI PFNGLUSEPROGRAMPROC glad_glUseProgram;
#define glUseProgram glad_glUseProgram
GLAPI PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
#define glGetProgramiv glad_glGetProgramiv
GLAPI PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
#define glGetProgramInfoLog glad_glGetProgramInfoLog
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
#define glGetUniformLocation glad_glGetUniformLocation

GLAPI PFNGLUNIFORM1FPROC glad_glUniform1f;
#define glUniform1f glad_glUniform1f
GLAPI PFNGLUNIFORM2FPROC glad_glUniform2f;
#define glUniform2f glad_glUniform2f
GLAPI PFNGLUNIFORM3FPROC glad_glUniform3f;
#define glUniform3f glad_glUniform3f
GLAPI PFNGLUNIFORM4FPROC glad_glUniform4f;
#define glUniform4f glad_glUniform4f
GLAPI PFNGLUNIFORM1IPROC glad_glUniform1i;
#define glUniform1i glad_glUniform1i

GLAPI PFNGLDRAWARRAYSPROC glad_glDrawArrays;
#define glDrawArrays glad_glDrawArrays
GLAPI PFNGLDRAWELEMENTSPROC glad_glDrawElements;
#define glDrawElements glad_glDrawElements
GLAPI PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
#define glDrawArraysInstanced glad_glDrawArraysInstanced
GLAPI PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced;
#define glDrawElementsInstanced glad_glDrawElementsInstanced

#ifdef __cplusplus
}
#endif

#endif
