cmake_minimum_required(VERSION 3.15)
project(OHLCVViewer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# Source files
set(COMMON_SOURCES
    src/common/DateTime.cpp
    src/common/TradingSession.cpp
)

set(BACKEND_SOURCES
    src/backend/CSVDataLoader.cpp
    src/backend/DataRepository.cpp
)

set(FRONTEND_SOURCES
    src/frontend/chart/ChartPanel.cpp
    src/frontend/chart/CandleRenderer.cpp
    src/frontend/chart/PriceAxis.cpp
    src/frontend/chart/TimeAxis.cpp
    src/frontend/chart/ChartScaler.cpp
    src/frontend/chart/gl/GLContext.cpp
    src/frontend/chart/gl/GLCandleRenderer.cpp
    src/frontend/ui/MainWindow.cpp
    src/frontend/ui/ControlPanel.cpp
)

set(GLAD_SOURCES
    third_party/glad/src/glad.c
)

set(MAIN_SOURCES
    src/main.cpp
)

# Create executable (WIN32 = GUI app, not console)
add_executable(${PROJECT_NAME} WIN32
    ${MAIN_SOURCES}
    ${COMMON_SOURCES}
    ${BACKEND_SOURCES}
    ${FRONTEND_SOURCES}
    ${GLAD_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

# Link Windows libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    user32
    gdi32
    comctl32
    comdlg32
    dwmapi
    OpenGL::GL
)

# Enable modern common controls (Windows 7+)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _WIN32_WINNT=0x0601
)

# Sample data generator (console tool)
add_executable(generate_sample_data
    tools/generate_sample_data_main.cpp
    tools/SampleDataGenerator.cpp
    src/common/DateTime.cpp
)
target_include_directories(generate_sample_data PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tools
)

# Copy data files to build directory for easier testing
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

# Generate sample data next to the viewer so the app works out of the box
add_dependencies(${PROJECT_NAME} generate_sample_data)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND $<TARGET_FILE:generate_sample_data>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/data/sample_ohlcv_1m.csv
)

# Unit tests
option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # Use the same CRT as the rest of the project (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(unit_tests
        tests/DateTimeTests.cpp
        tests/ChartScalerTests.cpp
        tests/CSVDataLoaderTests.cpp
        tests/DataRepositoryTests.cpp
        tests/SampleDataGeneratorTests.cpp
        tools/SampleDataGenerator.cpp
        src/frontend/chart/ChartScaler.cpp
        ${COMMON_SOURCES}
        ${BACKEND_SOURCES}
    )

    target_include_directories(unit_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tools
    )

    target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()
