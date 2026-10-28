cmake_minimum_required(VERSION 3.14)
project(westeros_gl_brcm_l1 LANGUAGES CXX)

include(CTest)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

option(ENABLE_COVERAGE "Instrument production and test code for gcovr" OFF)
get_filename_component(WESTEROS_GL_BRCM_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." REALPATH)
set(WESTEROS_GL_BRCM_SOURCE "${WESTEROS_GL_BRCM_ROOT}/westeros-gl/westeros-gl.cpp")

add_executable(westeros_gl_brcm_l1_tests
    tests/westeros_gl_brcm_test.cpp
    mocks/nexus_mock.cpp
    "${WESTEROS_GL_BRCM_SOURCE}"
)

target_include_directories(westeros_gl_brcm_l1_tests PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/mocks"
    "${WESTEROS_GL_BRCM_ROOT}/westeros-gl"
)
target_compile_definitions(westeros_gl_brcm_l1_tests PRIVATE UNIT_TEST)
target_link_libraries(westeros_gl_brcm_l1_tests PRIVATE GTest::gtest_main Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(westeros_gl_brcm_l1_tests PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ENABLE_COVERAGE requires GCC or Clang gcov-compatible instrumentation")
    endif()
    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        message(FATAL_ERROR "ENABLE_COVERAGE requires gcovr")
    endif()
    target_compile_options(westeros_gl_brcm_l1_tests PRIVATE --coverage -O0 -g)
    target_link_options(westeros_gl_brcm_l1_tests PRIVATE --coverage)

    add_custom_target(coverage
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/coverage_html"
        COMMAND "${GCOVR_EXECUTABLE}"
            --root "${WESTEROS_GL_BRCM_ROOT}"
            --filter "${WESTEROS_GL_BRCM_ROOT}/westeros-gl/westeros-gl.cpp"
            --html-details "${CMAKE_BINARY_DIR}/coverage_html/index.html"
            --xml "${CMAKE_BINARY_DIR}/coverage.xml"
            --print-summary
            --fail-under-line 75
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS westeros_gl_brcm_l1_tests
        COMMENT "Measure production line coverage for westeros-gl.cpp (75% minimum)"
        VERBATIM
    )
endif()

if(BUILD_TESTING)
    add_test(NAME WesterosGLBrcmL1Tests COMMAND westeros_gl_brcm_l1_tests)
    set_tests_properties(WesterosGLBrcmL1Tests PROPERTIES
        TIMEOUT 120
        LABELS "L1;WesterosGLBRCM;Component"
    )
endif()
