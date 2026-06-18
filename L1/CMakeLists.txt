# L1 Test Suite for Westeros GL BRCM
# CMake configuration for L1 tests with mocks
# Target: 75%+ code coverage

cmake_minimum_required(VERSION 3.14)

project(WesterosGLBrcmL1Tests
    VERSION 1.0.0
    DESCRIPTION "L1 Test Suite for Westeros GL BRCM with Mocks"
    LANGUAGES C CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(ENABLE_COVERAGE "Enable code coverage" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -lgcov")
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Wayland (for headers)
pkg_check_modules(WAYLAND REQUIRED wayland-client>=1.6.0 wayland-server>=1.6.0)

# Include directories - mocks MUST be first to override system EGL headers
include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../westeros-gl
    ${CMAKE_CURRENT_SOURCE_DIR}/../westeros-render-nexus
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
include_directories(SYSTEM ${WAYLAND_INCLUDE_DIRS})

# Mock source files
set(MOCK_SOURCES
    mocks/mock_nexus.cpp
    mocks/mock_nxclient.cpp
    mocks/mock_nxpl.cpp
    mocks/mock_egl.cpp
    mocks/mock_wayland_server.cpp
    mocks/mock_westeros_simplebuffer.cpp
)

# Mock library
add_library(westeros_gl_mocks STATIC ${MOCK_SOURCES})
target_include_directories(westeros_gl_mocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${WAYLAND_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
)

# Source to test (westeros-gl)
set(WESTEROS_GL_TEST_SOURCES
    ../westeros-gl/westeros-gl.cpp
)

# Source to test (westeros-render-nexus)
set(WESTEROS_RENDERER_TEST_SOURCES
    ../westeros-render-nexus/westeros-render-nexus.cpp
)

# Test source files for westeros-gl
set(WESTEROS_GL_TEST_FILES
    tests/L1_westeros_gl_init_tests.cpp
    tests/L1_westeros_gl_display_tests.cpp
    tests/L1_westeros_gl_window_tests.cpp
    tests/L1_westeros_gl_pixmap_tests.cpp
    tests/L1_westeros_gl_coverage_tests.cpp
)

# Test source files for westeros-render-nexus
set(WESTEROS_RENDERER_TEST_FILES
    tests/L1_westeros_renderer_init_tests.cpp
    tests/L1_westeros_renderer_surface_tests.cpp
    tests/L1_westeros_renderer_properties_tests.cpp
    tests/L1_westeros_renderer_coverage_tests.cpp
)

# Combined test sources
set(TEST_SOURCES
    ${WESTEROS_GL_TEST_FILES}
    ${WESTEROS_RENDERER_TEST_FILES}
    tests/test_main.cpp
)

# Test executable for westeros-gl
add_executable(westeros_gl_l1_tests
    ${WESTEROS_GL_TEST_FILES}
    tests/test_main.cpp
    ${WESTEROS_GL_TEST_SOURCES}
)

target_link_libraries(westeros_gl_l1_tests
    westeros_gl_mocks
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
    dl
)

target_compile_definitions(westeros_gl_l1_tests PRIVATE
    L1_TESTING=1
    NEXUS_PLATFORM_VERSION_MAJOR=18
    WESTEROS_HAVE_BRCM_WAYLAND_EGL=1
)

# Test executable for westeros-render-nexus
add_executable(westeros_renderer_l1_tests
    ${WESTEROS_RENDERER_TEST_FILES}
    tests/test_main.cpp
    ${WESTEROS_RENDERER_TEST_SOURCES}
)

target_include_directories(westeros_renderer_l1_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../westeros-gl
    ${CMAKE_CURRENT_SOURCE_DIR}/../westeros-render-nexus
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_link_libraries(westeros_renderer_l1_tests
    westeros_gl_mocks
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
    dl
)

target_compile_definitions(westeros_renderer_l1_tests PRIVATE
    L1_TESTING=1
    NEXUS_PLATFORM_VERSION_MAJOR=18
    WESTEROS_HAVE_BRCM_WAYLAND_EGL=1
    ENABLE_SBPROTOCOL=1
)

# Enable testing
enable_testing()

# Add tests
add_test(NAME WesterosGLBrcmL1Tests COMMAND westeros_gl_l1_tests)
add_test(NAME WesterosRendererL1Tests COMMAND westeros_renderer_l1_tests)

# Coverage target
if(ENABLE_COVERAGE)
    # Find required tools
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Found lcov: ${LCOV_PATH}")
        message(STATUS "Found genhtml: ${GENHTML_PATH}")
        
        add_custom_target(coverage
            # Clean previous coverage data
            COMMAND ${LCOV_PATH} --zerocounters --directory .
            COMMAND ${CMAKE_COMMAND} -E remove -f coverage.info coverage_filtered.info
            
            # Run GL tests first
            COMMAND ${CMAKE_COMMAND} -E echo "Running westeros_gl_l1_tests..."
            COMMAND ./westeros_gl_l1_tests || true
            
            # Capture GL coverage
            COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage_gl.info 
                    --rc branch_coverage=1 
                    --rc geninfo_unexecuted_blocks=1
                    --ignore-errors gcov,mismatch
            COMMAND ${CMAKE_COMMAND} -E echo "GL coverage captured"
            
            # Run Renderer tests
            COMMAND ${CMAKE_COMMAND} -E echo "Running westeros_renderer_l1_tests..."
            COMMAND ./westeros_renderer_l1_tests || true
            
            # Capture Renderer coverage
            COMMAND ${LCOV_PATH} --capture --directory . --output-file coverage_renderer.info 
                    --rc branch_coverage=1 
                    --rc geninfo_unexecuted_blocks=1
                    --ignore-errors gcov,mismatch
            COMMAND ${CMAKE_COMMAND} -E echo "Renderer coverage captured"
            
            # Combine coverage files
            COMMAND ${LCOV_PATH} --add-tracefile coverage_gl.info --add-tracefile coverage_renderer.info 
                    --output-file coverage.info 
                    --rc branch_coverage=1
                    --ignore-errors mismatch
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage data combined"
            
            # Filter out system headers and test files
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/tests/*' '*/mocks/*' 
                    --output-file coverage_filtered.info 
                    --rc branch_coverage=1
                    --ignore-errors mismatch,unused
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage data filtered"
            
            # Generate HTML report
            COMMAND ${GENHTML_PATH} coverage_filtered.info 
                    --output-directory coverage_html 
                    --title "Westeros GL + Renderer L1 Coverage Report" 
                    --legend 
                    --branch-coverage
                    --demangle-cpp
                    --ignore-errors source
            COMMAND ${CMAKE_COMMAND} -E echo "HTML report generated at: ${CMAKE_BINARY_DIR}/coverage_html/index.html"
            
            # Display summary
            COMMAND ${LCOV_PATH} --list coverage_filtered.info 
                    --rc branch_coverage=1
                    --ignore-errors mismatch
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS westeros_gl_l1_tests westeros_renderer_l1_tests
            COMMENT "Generating combined code coverage report for westeros-gl and westeros-render-nexus"
        )
        
        message(STATUS "Coverage target added. Run: make coverage")
    else()
        if(NOT LCOV_PATH)
            message(WARNING "lcov not found. Install with: sudo apt-get install lcov")
        endif()
        if(NOT GENHTML_PATH)
            message(WARNING "genhtml not found. Install with: sudo apt-get install lcov")
        endif()
        message(WARNING "Coverage target not available")
    endif()
endif()

# Install test executables
install(TARGETS westeros_gl_l1_tests westeros_renderer_l1_tests
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "========================================")
message(STATUS "Westeros GL + Renderer L1 Test Suite Configuration")
message(STATUS "========================================")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Mock sources: ${MOCK_SOURCES}")
message(STATUS "Westeros GL test sources: ${WESTEROS_GL_TEST_FILES}")
message(STATUS "Westeros Renderer test sources: ${WESTEROS_RENDERER_TEST_FILES}")
message(STATUS "Total test count: 2 test suites (GL + Renderer)")
message(STATUS "========================================")
