# L1 Test Suite for Westeros GL BRCM

## Overview
This directory contains L1 (Level 1) unit tests for both **westeros-gl** and **westeros-render-nexus** libraries, targeting **70%+ code coverage** for each component.

## Test Components

### Westeros GL Test Files
- `L1_westeros_gl_init_tests.cpp` - Tests for initialization and termination
- `L1_westeros_gl_display_tests.cpp` - Tests for display information and sizing  
- `L1_westeros_gl_window_tests.cpp` - Tests for native window creation/destruction
- `L1_westeros_gl_pixmap_tests.cpp` - Tests for pixmap operations
- `L1_westeros_gl_coverage_tests.cpp` - Additional tests for coverage boost

### Westeros Renderer Test Files (NEW)
- `L1_westeros_renderer_init_tests.cpp` - Renderer initialization and termination
- `L1_westeros_renderer_surface_tests.cpp` - Surface creation, destruction, commit
- `L1_westeros_renderer_properties_tests.cpp` - Surface properties (visibility, geometry, opacity, z-order)
- `L1_westeros_renderer_coverage_tests.cpp` - Comprehensive edge cases and error paths

### Common Test File
- `test_main.cpp` - Main test runner for both test suites

### Mock Files
- `mock_nexus.cpp` - Mock Nexus platform APIs + SurfaceClient APIs
- `mock_nxclient.cpp` - Mock NxClient APIs
- `mock_nxpl.cpp` - Mock NXPL (Nexus Platform Layer) APIs
- `mock_egl.cpp` - Mock EGL APIs
- `mock_wayland_server.cpp` - Mock Wayland server APIs (NEW)

### Mock Headers
- `westeros-render.h` - Renderer interface definition (NEW)
- `wayland-server.h` - Wayland server types (NEW)
- `westeros-simplebuffer.h` - Simple buffer protocol (NEW)

## Building

```bash
# Configure and build
cd L1
mkdir -p build
cd build
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
make

# Run westeros-gl tests
./westeros_gl_l1_tests

# Run westeros-renderer tests
./westeros_renderer_l1_tests

# Run all tests
ctest --output-on-failure

# Generate combined coverage report
make coverage
```

## Coverage Target
- **Target:** 70%+ code coverage for both westeros-gl.cpp and westeros-render-nexus.cpp
- **Includes:** 
  - All 13 public functions in westeros-gl.cpp
  - All 18 renderer functions in westeros-render-nexus.cpp
- **Reports:** HTML and LCOV format

## Test Categories

### Westeros GL Tests
1. **Initialization Tests** - Context creation, initialization, termination
2. **Display Tests** - Display info, size, safe area, listeners
3. **Window Tests** - Native window creation and destruction
4. **Pixmap Tests** - Pixmap operations and EGL integration
5. **Error Handling** - NULL pointer checks, invalid parameters
6. **Thread Safety** - Mutex locking, concurrent operations
7. **Memory Management** - Allocation, deallocation, leak prevention

### Westeros Renderer Tests (NEW)
1. **Initialization Tests** - renderer_init(), wstRendererTerm(), NxClient join scenarios
2. **Surface Management** - Create, destroy, commit, update scene
3. **Surface Properties** - Visibility, geometry, opacity, z-order (get/set)
4. **Scene Updates** - updateScene(), delegateUpdateScene() with rectangles
5. **Error Paths** - NULL parameters, push failures, invalid arguments
6. **Boundary Testing** - Extreme geometry values, opacity/z-order limits
7. **Stress Testing** - Many surfaces, rapid commits, property changes

## Requirements
- GTest/GMock
- CMake 3.14+
- lcov (for coverage)
- Mock Nexus libraries

## Test Executables
- **westeros_gl_l1_tests** - 78+ test cases for westeros-gl.cpp
- **westeros_renderer_l1_tests** - 60+ test cases for westeros-render-nexus.cpp
- **Total:** 138+ comprehensive test cases

## CI/CD Integration
- GitHub Actions workflow included
- Automated testing on push/PR
- Coverage reports uploaded as artifacts
- Combined coverage for both components