#pragma once

#include "../ChartScaler.h"
#include "../../../common/OHLCVBar.h"
#include <glad/glad.h>
#include <string>
#include <vector>

namespace Origin {

class GLCandleRenderer {
public:
    GLCandleRenderer() = default;
    ~GLCandleRenderer();

    bool initialize();
    void shutdown();

    void setScaler(const ChartScaler* scaler) { m_scaler = scaler; }
    void render(const OHLCVData& bars, int viewportWidth, int viewportHeight);

    // Shader compile/link error from the last failed initialize(), empty on success
    const std::string& getLastError() const { return m_lastError; }

private:
    bool compileShaders();
    void createGeometry();
    void updateInstanceBuffer(const OHLCVData& bars, int firstVisible, int lastVisible);

    const ChartScaler* m_scaler = nullptr;

    GLuint m_shaderProgram = 0;
    GLuint m_wickVAO = 0;
    GLuint m_wickVBO = 0;
    GLuint m_bodyVAO = 0;
    GLuint m_bodyVBO = 0;
    GLuint m_bodyEBO = 0;
    GLuint m_instanceVBO = 0;

    // Uniform locations
    GLint m_uViewportSize = -1;
    GLint m_uChartBounds = -1;
    GLint m_uPriceRange = -1;
    GLint m_uBarParams = -1;
    GLint m_uBullishColor = -1;
    GLint m_uBearishColor = -1;
    GLint m_uDrawWick = -1;

    std::string m_lastError;
    std::vector<float> m_instanceData;
    int m_instanceCount = 0;
    int m_cachedFirstVisible = -1;
    int m_cachedLastVisible = -1;
};

} // namespace Origin
