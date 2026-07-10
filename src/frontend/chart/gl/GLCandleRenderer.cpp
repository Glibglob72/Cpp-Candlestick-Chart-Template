#include "GLCandleRenderer.h"
#include <algorithm>
#include <cstring>

namespace Origin {

static const char* VERTEX_SHADER = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in float a_barIndex;
layout(location = 2) in float a_open;
layout(location = 3) in float a_high;
layout(location = 4) in float a_low;
layout(location = 5) in float a_close;
layout(location = 6) in float a_isBullish;

uniform vec2 u_viewportSize;
uniform vec4 u_chartBounds;
uniform vec2 u_priceRange;
uniform vec3 u_barParams;
uniform int u_drawWick;

flat out float v_isBullish;

float priceToY(float price) {
    float chartHeight = u_chartBounds.w - u_chartBounds.z;
    float ratio = (price - u_priceRange.x) / (u_priceRange.y - u_priceRange.x);
    return u_chartBounds.w - ratio * chartHeight;
}

float barIndexToX(float index) {
    float chartWidth = u_chartBounds.y - u_chartBounds.x;
    float pixelsPerBar = chartWidth / u_barParams.x;
    return u_chartBounds.x + index * pixelsPerBar + u_barParams.y;
}

void main() {
    float x = barIndexToX(a_barIndex);
    float barWidth = u_barParams.z;
    float centerX = x + barWidth / 2.0;

    float highY = priceToY(a_high);
    float lowY = priceToY(a_low);
    float openY = priceToY(a_open);
    float closeY = priceToY(a_close);

    float bodyTop = min(openY, closeY);
    float bodyBottom = max(openY, closeY);
    if (bodyTop == bodyBottom) bodyBottom = bodyTop + 1.0;

    vec2 pos;
    if (u_drawWick == 1) {
        pos.x = centerX;
        pos.y = mix(highY, lowY, a_position.y);
    } else {
        float bodyWidth = max(1.0, barWidth - 2.0);
        float bodyLeft = centerX - bodyWidth / 2.0;
        pos.x = bodyLeft + a_position.x * bodyWidth;
        pos.y = mix(bodyTop, bodyBottom, a_position.y);
    }

    vec2 ndc;
    ndc.x = (pos.x / u_viewportSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (pos.y / u_viewportSize.y) * 2.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    v_isBullish = a_isBullish;
}
)";

static const char* FRAGMENT_SHADER = R"(
#version 330 core
flat in float v_isBullish;
uniform vec3 u_bullishColor;
uniform vec3 u_bearishColor;
out vec4 fragColor;

void main() {
    vec3 color = mix(u_bearishColor, u_bullishColor, v_isBullish);
    fragColor = vec4(color, 1.0);
}
)";

GLCandleRenderer::~GLCandleRenderer() {
    shutdown();
}

bool GLCandleRenderer::initialize() {
    if (!compileShaders()) return false;
    createGeometry();
    return true;
}

void GLCandleRenderer::shutdown() {
    if (m_shaderProgram) { glDeleteProgram(m_shaderProgram); m_shaderProgram = 0; }
    if (m_wickVAO) { glDeleteVertexArrays(1, &m_wickVAO); m_wickVAO = 0; }
    if (m_wickVBO) { glDeleteBuffers(1, &m_wickVBO); m_wickVBO = 0; }
    if (m_bodyVAO) { glDeleteVertexArrays(1, &m_bodyVAO); m_bodyVAO = 0; }
    if (m_bodyVBO) { glDeleteBuffers(1, &m_bodyVBO); m_bodyVBO = 0; }
    if (m_bodyEBO) { glDeleteBuffers(1, &m_bodyEBO); m_bodyEBO = 0; }
    if (m_instanceVBO) { glDeleteBuffers(1, &m_instanceVBO); m_instanceVBO = 0; }
}

static std::string getShaderInfoLog(GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) return "(no info log)";
    std::string log(length - 1, '\0');
    glGetShaderInfoLog(shader, length, nullptr, log.data());
    return log;
}

static std::string getProgramInfoLog(GLuint program) {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length <= 1) return "(no info log)";
    std::string log(length - 1, '\0');
    glGetProgramInfoLog(program, length, nullptr, log.data());
    return log;
}

bool GLCandleRenderer::compileShaders() {
    m_lastError.clear();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &VERTEX_SHADER, nullptr);
    glCompileShader(vs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        m_lastError = "Vertex shader compile error: " + getShaderInfoLog(vs);
        glDeleteShader(vs);
        return false;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &FRAGMENT_SHADER, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        m_lastError = "Fragment shader compile error: " + getShaderInfoLog(fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!success) {
        m_lastError = "Shader program link error: " + getProgramInfoLog(m_shaderProgram);
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
        return false;
    }

    m_uViewportSize = glGetUniformLocation(m_shaderProgram, "u_viewportSize");
    m_uChartBounds = glGetUniformLocation(m_shaderProgram, "u_chartBounds");
    m_uPriceRange = glGetUniformLocation(m_shaderProgram, "u_priceRange");
    m_uBarParams = glGetUniformLocation(m_shaderProgram, "u_barParams");
    m_uBullishColor = glGetUniformLocation(m_shaderProgram, "u_bullishColor");
    m_uBearishColor = glGetUniformLocation(m_shaderProgram, "u_bearishColor");
    m_uDrawWick = glGetUniformLocation(m_shaderProgram, "u_drawWick");

    return true;
}

void GLCandleRenderer::createGeometry() {
    // Instance buffer (shared)
    glGenBuffers(1, &m_instanceVBO);

    // Wick: line from (0,0) to (0,1)
    float wickVerts[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGenVertexArrays(1, &m_wickVAO);
    glGenBuffers(1, &m_wickVBO);
    glBindVertexArray(m_wickVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_wickVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wickVerts), wickVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // Bind instance buffer for wick VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    for (int i = 1; i <= 6; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)((i - 1) * sizeof(float)));
        glVertexAttribDivisor(i, 1);
    }

    // Body: quad (0,0), (1,0), (1,1), (0,1)
    float bodyVerts[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    unsigned int bodyIndices[] = { 0, 1, 2, 0, 2, 3 };
    glGenVertexArrays(1, &m_bodyVAO);
    glGenBuffers(1, &m_bodyVBO);
    glGenBuffers(1, &m_bodyEBO);
    glBindVertexArray(m_bodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_bodyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bodyVerts), bodyVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bodyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bodyIndices), bodyIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // Bind instance buffer for body VAO
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    for (int i = 1; i <= 6; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)((i - 1) * sizeof(float)));
        glVertexAttribDivisor(i, 1);
    }

    glBindVertexArray(0);
}

void GLCandleRenderer::updateInstanceBuffer(const OHLCVData& bars, int firstVisible, int lastVisible) {
    m_instanceCount = lastVisible - firstVisible + 1;
    m_instanceData.resize(m_instanceCount * 6);

    for (int i = firstVisible; i <= lastVisible; ++i) {
        const OHLCVBar& bar = bars[i];
        int idx = (i - firstVisible) * 6;
        m_instanceData[idx + 0] = static_cast<float>(i);
        m_instanceData[idx + 1] = static_cast<float>(bar.open);
        m_instanceData[idx + 2] = static_cast<float>(bar.high);
        m_instanceData[idx + 3] = static_cast<float>(bar.low);
        m_instanceData[idx + 4] = static_cast<float>(bar.close);
        m_instanceData[idx + 5] = bar.isBullish() ? 1.0f : 0.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instanceData.size() * sizeof(float), m_instanceData.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLCandleRenderer::render(const OHLCVData& bars, int viewportWidth, int viewportHeight) {
    if (!m_scaler || bars.empty() || !m_shaderProgram) return;

    int dataSize = static_cast<int>(bars.size());

    // Calculate visible range based on scroll and bar count
    int barCount = m_scaler->getBarCount();
    if (barCount <= 0) barCount = dataSize;

    double chartWidth = static_cast<double>(m_scaler->getChartWidth());
    double pixelsPerBar = (barCount > 0) ? chartWidth / barCount : 1.0;
    int scrollOffset = m_scaler->getScrollOffset();

    // First visible bar: where does the chart area start in bar-space?
    int firstVisible = static_cast<int>(-scrollOffset / pixelsPerBar);
    firstVisible = std::max(0, firstVisible - 1); // -1 for safety margin

    // Last visible bar: where does the chart area end in bar-space?
    int lastVisible = static_cast<int>((-scrollOffset + chartWidth) / pixelsPerBar);
    lastVisible = std::min(dataSize - 1, lastVisible + 1); // +1 for safety margin

    if (firstVisible > lastVisible || firstVisible >= dataSize) return;

    if (firstVisible != m_cachedFirstVisible || lastVisible != m_cachedLastVisible) {
        updateInstanceBuffer(bars, firstVisible, lastVisible);
        m_cachedFirstVisible = firstVisible;
        m_cachedLastVisible = lastVisible;
    }

    glUseProgram(m_shaderProgram);
    glUniform2f(m_uViewportSize, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));
    glUniform4f(m_uChartBounds,
        static_cast<float>(m_scaler->getChartLeft()),
        static_cast<float>(m_scaler->getChartRight()),
        static_cast<float>(m_scaler->getChartTop()),
        static_cast<float>(m_scaler->getChartBottom()));
    glUniform2f(m_uPriceRange,
        static_cast<float>(m_scaler->getMinPrice()),
        static_cast<float>(m_scaler->getMaxPrice()));
    glUniform3f(m_uBarParams,
        static_cast<float>(m_scaler->getBarCount()),
        static_cast<float>(m_scaler->getScrollOffset()),
        static_cast<float>(m_scaler->getBarWidth()));
    glUniform3f(m_uBullishColor, 38.0f/255.0f, 166.0f/255.0f, 91.0f/255.0f);
    glUniform3f(m_uBearishColor, 214.0f/255.0f, 69.0f/255.0f, 65.0f/255.0f);

    // Draw wicks
    glUniform1i(m_uDrawWick, 1);
    glBindVertexArray(m_wickVAO);
    glDrawArraysInstanced(GL_LINES, 0, 2, m_instanceCount);

    // Draw bodies
    glUniform1i(m_uDrawWick, 0);
    glBindVertexArray(m_bodyVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, m_instanceCount);

    glBindVertexArray(0);
    glUseProgram(0);
}

} // namespace Origin
