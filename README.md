# Westeros GL BRCM - Complete L1 Test Suite and Coverity Integration

## Overview
This repository contains the Westeros GL implementation for Broadcom platforms with comprehensive L1 (Level 1) unit testing and Coverity static analysis integration. The test suite targets **75%+ code coverage** and follows the same architecture as the essos-main project.

## Repository Structure
```
westeros-gl-brcm-main/
├── westeros-gl/              # Main GL implementation
│   ├── westeros-gl.cpp       # Core implementation (775 lines)
│   └── westeros-gl.h         # Public API (13 functions)
├── westeros-render-nexus/    # Broadcom Nexus renderer
│   └── westeros-render-nexus.cpp
├── L1/                       # L1 test suite (NEW)
│   ├── CMakeLists.txt        # Build configuration
│   ├── README.md             # L1 documentation
│   ├── mocks/                # Mock implementations
│   │   ├── mock_nexus.cpp    # NEXUS Graphics2D mocks
│   │   ├── mock_nxclient.cpp # NxClient API mocks
│   │   ├── mock_nxpl.cpp     # NXPL platform mocks
│   │   └── mock_egl.cpp      # EGL API mocks
│   └── tests/                # Test suites
│       ├── L1_westeros_gl_init_tests.cpp      # Init/term tests
│       ├── L1_westeros_gl_display_tests.cpp   # Display tests
│       ├── L1_westeros_gl_window_tests.cpp    # Window tests
│       ├── L1_westeros_gl_pixmap_tests.cpp    # Pixmap tests
│       ├── L1_westeros_gl_coverage_tests.cpp  # Coverage boost
│       └── test_main.cpp                      # Test runner
├── scripts/                  # Build and analysis scripts (NEW)
│   ├── setup.sh              # Dependency installation
│   └── build.sh              # Unified build (L1 + Coverity)
├── .github/workflows/        # CI/CD workflows (NEW)
│   ├── l1-tests.yml          # L1 test automation
│   └── coverity-scan.yml     # Coverity integration
├── coverity-config.yml       # Coverity configuration (NEW)
└── docs/                     # Documentation
    └── relnotes/             # Release notes
```

## Features

### L1 Test Suite
The L1 test suite provides comprehensive unit testing with the following components:

#### Test Coverage Areas
1. **Initialization Tests** (`L1_westeros_gl_init_tests.cpp`)
   - Context lifecycle (init/term)
   - Multiple initialization cycles
   - NxClient join scenarios
   - Graphics2D resource allocation
   - Error handling for initialization failures

2. **Display Tests** (`L1_westeros_gl_display_tests.cpp`)
   - Display information retrieval
   - Safe area queries
   - Display size listeners
   - Listener management (add/remove)
   - Multiple listener support

3. **Window Tests** (`L1_westeros_gl_window_tests.cpp`)
   - Native window creation/destruction
   - Various window sizes and positions
   - Multiple window management
   - Error handling (NULL checks, invalid params)
   - Stress testing (100+ operations)

4. **Pixmap Tests** (`L1_westeros_gl_pixmap_tests.cpp`)
   - Pixmap allocation/release
   - Dimension queries
   - EGL pixmap conversion
   - Multiple pixmap management
   - Resource lifecycle testing

5. **Coverage Boost Tests** (`L1_westeros_gl_coverage_tests.cpp`)
   - MEGA lifecycle tests
   - Boundary value testing
   - Error path coverage
   - Resource exhaustion scenarios
   - Mixed operation stress tests

#### Mock System
Complete mock implementations for hardware-dependent APIs:
- **NEXUS Graphics2D**: Surface creation, blitting, memory management
- **NxClient**: Display settings, surface client operations
- **NXPL**: Platform and native window operations
- **EGL**: Display initialization and surface management

### Build System

#### CMake Configuration
- Modern CMake (3.14+)
- Coverage support with `--coverage` flags
- Modular mock library
- Automated coverage report generation (HTML + LCOV)
- GTest/GMock integration

#### Build Scripts
- **setup.sh**: Automated dependency installation and verification
- **build.sh**: Unified build script with options for L1 tests, Coverity analysis, or both

### CI/CD Integration

#### GitHub Actions Workflows
1. **L1 Tests Workflow** (`.github/workflows/l1-tests.yml`)
   - Multiple build configurations (Debug/Release, Coverage ON/OFF)
   - Automated test execution
   - Coverage threshold enforcement (70%)
   - Artifact uploads (test results, coverage reports)
   - Matrix builds for comprehensive testing

2. **Coverity Scan Workflow** (`.github/workflows/coverity-scan.yml`)
   - Weekly automated scans
   - Manual trigger support
   - Result archiving
   - Integration with Coverity Scan service

### Coverity Integration

#### Configuration
- Comprehensive checker enablement
- Security and concurrency analysis
- Configurable severity thresholds
- HTML and text report generation
- CI/CD integration with failure policies

#### Local Analysis
Run Coverity locally before committing:
```bash
./scripts/build.sh --coverity
```

Results available in `coverity-output/html-report/index.html`

## Getting Started

### Prerequisites
- Ubuntu 22.04+ (or compatible Linux)
- CMake 3.14+
- GCC/G++
- GTest/GMock
- Wayland development files
- lcov, gcovr (for coverage)
- Coverity Build Tool (for static analysis)

### Quick Start

1. **Install Dependencies**
   ```bash
   cd westeros-gl-brcm-main
   ./scripts/setup.sh
   ```

### 2. **Build L1 Tests**
   ```bash
   ./scripts/build.sh --l1
   ```

3. **Build for Coverity Analysis**
   ```bash
   ./scripts/build.sh --coverity
   ```

4. **Build Both**
   ```bash
   ./scripts/build.sh --both
   # or simply
   ./scripts/build.sh
   ```

### Build Options

**Build L1 tests with coverage:**
```bash
./scripts/build.sh --l1
```

**Build for Coverity analysis:**
```bash
./scripts/build.sh --coverity
```

**Build both L1 and Coverity:**
```bash
./scripts/build.sh --both
```

**Clean build:**
```bash
./scripts/build.sh --l1 --clean
```

**Verbose output:**
```bash
./scripts/build.sh --both --verbose
```

## Test Architecture

### Design Principles
The L1 test suite follows industry best practices:

1. **Isolation**: Each test is independent using GTest fixtures
2. **Comprehensive**: Covers normal paths, error paths, and edge cases
3. **Maintainable**: Clear naming, documentation, and organization
4. **Fast**: Mock-based for no hardware dependencies
5. **Deterministic**: Repeatable results on any platform

### Test Patterns
- **SetUp/TearDown**: Consistent test initialization/cleanup
- **Assertions**: ASSERT for preconditions, EXPECT for verifications
- **Mock Control**: External functions to manipulate mock behavior
- **Resource Tracking**: Verify proper allocation/deallocation

### Coverage Strategy
The suite achieves 75%+ coverage through:
- Positive and negative test cases
- Boundary value analysis
- Error injection (allocation failures, NULL pointers)
- Stress testing (resource exhaustion, rapid allocation/deallocation)
- Integration tests (full API workflows)

## API Coverage

All 13 public API functions are comprehensively tested:

| Function | Test File | Coverage |
|----------|-----------|----------|
| `WstGLInit` | init_tests.cpp | ✅ Full |
| `WstGLTerm` | init_tests.cpp | ✅ Full |
| `WstGLGetDisplayInfo` | display_tests.cpp | ✅ Full |
| `WstGLGetDisplaySafeArea` | display_tests.cpp | ✅ Full |
| `WstGLAddDisplaySizeListener` | display_tests.cpp | ✅ Full |
| `WstGLRemoveDisplaySizeListener` | display_tests.cpp | ✅ Full |
| `WstGLCreateNativeWindow` | window_tests.cpp | ✅ Full |
| `WstGLDestroyNativeWindow` | window_tests.cpp | ✅ Full |
| `WstGLGetNativePixmap` | pixmap_tests.cpp | ✅ Full |
| `WstGLGetNativePixmapDimensions` | pixmap_tests.cpp | ✅ Full |
| `WstGLReleaseNativePixmap` | pixmap_tests.cpp | ✅ Full |
| `WstGLGetEGLNativePixmap` | pixmap_tests.cpp | ✅ Full |

## Continuous Integration

### Workflow Triggers
- **Push**: Automated testing on main/develop branches
- **Pull Request**: Required checks before merging
- **Schedule**: Weekly Coverity scans
- **Manual**: On-demand workflow dispatch

### Quality Gates
- All tests must pass
- Code coverage must be ≥ 70%
- Coverity defects reviewed and addressed
- No high-severity issues in production

## Contributing

When contributing to this project:

1. Run tests locally: `./scripts/build.sh --l1`
2. Verify coverage: Check `L1/build/coverage_html/index.html`
3. Run Coverity: `./scripts/build.sh --coverity`
4. Ensure all CI checks pass
5. Update tests for new functionality

## Coverage Reports

### Viewing Coverage Locally
After running tests with coverage:
```bash
cd L1/build
firefox coverage_html/index.html
# or
google-chrome coverage_html/index.html
```

### Coverage Metrics
The coverage report includes:
- Line coverage percentage
- Function coverage percentage
- Branch coverage statistics
- Per-file coverage details
- Untested line identification

### CI Coverage Artifacts
Coverage reports are uploaded as artifacts in GitHub Actions:
- HTML report (viewable in browser)
- LCOV format (machine-readable)

## Static Analysis

### Coverity Checkers Enabled
- Resource leaks
- Use-after-free
- NULL dereference
- Buffer overflow
- Integer overflow
- Deadlock detection
- Race conditions
- Uninitialized variables
- Memory corruption
- Security vulnerabilities

### Reviewing Results
Coverity results are available:
- **Locally**: `coverity-output/html-report/index.html`
- **CI/CD**: Uploaded as workflow artifacts
- **Coverity Scan**: Online dashboard (if configured)

## Troubleshoments

### Common Issues

**Tests fail to build:**
- Run `./scripts/setup.sh` to install dependencies
- Verify CMake version ≥ 3.14
- Check GTest/GMock installation

**Coverage below threshold:**
- Review untested code in coverage report
- Add tests to `L1_westeros_gl_coverage_tests.cpp`
- Verify mock behavior matches real APIs

**Coverity not found:**
- Download from https://scan.coverity.com/download
- Add to PATH: `export PATH=$PATH:/path/to/coverity/bin`

## License
See [LICENSE](LICENSE) and [COPYING](COPYING) files.

## Related Projects
- [essos-main](https://github.com/comcast/essos) - Reference L1 architecture
- [Westeros](https://github.com/rdkcmf/westeros) - Wayland compositor

## Contact
For issues and questions, please use the GitHub issue tracker.

---

**Version**: 2.0.0  
**Last Updated**: 2024  
**Coverage Target**: 75%+  
**Test Count**: 100+ test cases
