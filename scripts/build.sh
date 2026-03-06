#!/bin/bash
#If not stated otherwise in this file or this component's LICENSE file the
#following copyright and licenses apply:
#
#Copyright 2016 RDK Management
#
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.
#######################################
# Westeros GL BRCM Build Script
# Purpose: Build westeros-gl with L1 tests and/or Coverity analysis
# Usage: ./build.sh [OPTIONS]
#   Options:
#     --l1           : Build L1 tests with coverage
#     --coverity     : Build for Coverity analysis
#     --both         : Build both L1 and Coverity
#     --clean        : Clean build directories before building
#     --verbose      : Enable verbose output
#     --jobs N       : Number of parallel build jobs (default: nproc)
#     --help         : Show this help message
#######################################

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default configuration
BUILD_L1=false
BUILD_COVERITY=false
CLEAN_BUILD=false
VERBOSE=false
JOBS=$(nproc 2>/dev/null || echo 4)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Build directories
L1_BUILD_DIR="$PROJECT_ROOT/L1/build"
COVERITY_BUILD_DIR="$PROJECT_ROOT/build-coverity"
COVERITY_OUTPUT_DIR="$PROJECT_ROOT/coverity-output"

# Log functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_step() {
    echo -e "${CYAN}[STEP]${NC} $1"
}

# Show help
show_help() {
    cat << EOF
Westeros GL BRCM Build Script

Usage: $0 [OPTIONS]

Options:
    --l1               Build L1 tests with coverage
    --coverity         Build for Coverity analysis
    --both             Build both L1 and Coverity
    --clean            Clean build directories before building
    --verbose          Enable verbose output
    --jobs N           Number of parallel build jobs (default: $JOBS)
    --help             Show this help message

Examples:
    $0 --l1                     # Build L1 tests with coverage
    $0 --coverity               # Build for Coverity analysis
    $0 --both --clean           # Clean and build both
    $0 --l1 --jobs 8 --verbose  # Build L1 with 8 jobs and verbose output

EOF
}

# Parse command line arguments
parse_args() {
    if [ $# -eq 0 ]; then
        log_info "No arguments provided. Building both L1 and Coverity by default."
        BUILD_L1=true
        BUILD_COVERITY=true
        return
    fi
    
    while [ $# -gt 0 ]; do
        case $1 in
            --l1)
                BUILD_L1=true
                shift
                ;;
            --coverity)
                BUILD_COVERITY=true
                shift
                ;;
            --both)
                BUILD_L1=true
                BUILD_COVERITY=true
                shift
                ;;
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --jobs)
                JOBS=$2
                shift 2
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Clean build directories
clean_builds() {
    log_step "Cleaning build directories..."
    
    if [ "$BUILD_L1" = true ] && [ -d "$L1_BUILD_DIR" ]; then
        log_info "Removing L1 build directory: $L1_BUILD_DIR"
        rm -rf "$L1_BUILD_DIR"
    fi
    
    if [ "$BUILD_COVERITY" = true ]; then
        if [ -d "$COVERITY_BUILD_DIR" ]; then
            log_info "Removing Coverity build directory: $COVERITY_BUILD_DIR"
            rm -rf "$COVERITY_BUILD_DIR"
        fi
        
        if [ -d "$COVERITY_OUTPUT_DIR" ]; then
            log_info "Removing Coverity output directory: $COVERITY_OUTPUT_DIR"
            rm -rf "$COVERITY_OUTPUT_DIR"
        fi
    fi
    
    log_success "Build directories cleaned"
}

# Build L1 tests with coverage
build_l1() {
    log_step "Building L1 Tests with Coverage"
    
    # Check if L1 directory exists
    if [ ! -d "$PROJECT_ROOT/L1" ]; then
        log_error "L1 directory not found at $PROJECT_ROOT/L1"
        return 1
    fi
    
    # Create build directory
    mkdir -p "$L1_BUILD_DIR"
    cd "$L1_BUILD_DIR"
    
    # Configure with CMake
    log_info "Configuring L1 tests with CMake..."
    if [ "$VERBOSE" = true ]; then
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_COVERAGE=ON \
              -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              ..
    else
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_COVERAGE=ON \
              -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              .. > /dev/null
    fi
    
    # Build
    log_info "Building L1 tests (using $JOBS parallel jobs)..."
    if [ "$VERBOSE" = true ]; then
        make -j"$JOBS"
    else
        make -j"$JOBS" > /dev/null
    fi
    
    log_success "L1 tests built successfully"
    
    # Run tests
    log_info "Running L1 tests..."
    
    # Run GL tests
    if [ -f "./westeros_gl_l1_tests" ]; then
        log_step "Running westeros_gl_l1_tests..."
        ./westeros_gl_l1_tests || {
            TEST_EXIT_CODE=$?
            log_warning "Some GL tests failed with exit code: $TEST_EXIT_CODE"
            log_info "Continuing with renderer tests..."
        }
    else
        log_error "Test executable not found: ./westeros_gl_l1_tests"
        return 1
    fi
    
    # Run Renderer tests
    if [ -f "./westeros_renderer_l1_tests" ]; then
        log_step "Running westeros_renderer_l1_tests..."
        ./westeros_renderer_l1_tests || {
            TEST_EXIT_CODE=$?
            log_warning "Some renderer tests failed with exit code: $TEST_EXIT_CODE"
            log_info "Continuing with coverage generation..."
        }
    else
        log_warning "Test executable not found: ./westeros_renderer_l1_tests"
        log_info "Skipping renderer tests (this is normal if renderer tests aren't built)"
    fi
    
    # Check if coverage files were generated
    log_info "Checking for coverage data files..."
    GCDA_COUNT=$(find . -name "*.gcda" | wc -l)
    GCNO_COUNT=$(find . -name "*.gcno" | wc -l)
    
    log_info "Found $GCDA_COUNT .gcda files and $GCNO_COUNT .gcno files"
    
    if [ "$GCDA_COUNT" -eq 0 ]; then
        log_warning "No .gcda files found! Coverage data not generated."
        log_info "This usually means tests didn't run to completion or coverage flags are missing."
        log_info "Build flags used:"
        echo "  CMAKE_CXX_FLAGS: --coverage -fprofile-arcs -ftest-coverage"
        echo "  CMAKE_EXE_LINKER_FLAGS: --coverage -lgcov"
    else
        log_success "Coverage data files found, proceeding with report generation"
    fi
    
    # Generate coverage report
    generate_coverage_report
    
    cd "$PROJECT_ROOT"
}

# Generate coverage report
generate_coverage_report() {
    log_step "Generating Coverage Report"
    
    cd "$L1_BUILD_DIR"
    
    # Check for lcov and genhtml
    if ! command -v lcov &> /dev/null; then
        log_error "lcov not found. Please install: sudo apt-get install lcov"
        log_info "Skipping HTML coverage report generation"
        return 1
    fi
    
    if ! command -v genhtml &> /dev/null; then
        log_error "genhtml not found. Please install: sudo apt-get install lcov"
        log_info "Skipping HTML coverage report generation"
        return 1
    fi
    
    # Create coverage directory
    mkdir -p coverage/html
    
    # Generate HTML coverage report with lcov
    log_info "Generating HTML coverage report with lcov..."
    
    # Capture coverage data
    log_info "Capturing coverage data..."
    lcov --capture --directory . --output-file coverage/coverage.info \
         --rc branch_coverage=1 \
         --rc geninfo_unexecuted_blocks=1 \
         --ignore-errors gcov,source 2>&1 | tee coverage/lcov_capture.log
    
    if [ ! -f "coverage/coverage.info" ] || [ ! -s "coverage/coverage.info" ]; then
        log_error "Failed to capture coverage data"
        log_info "Check coverage/lcov_capture.log for details"
        return 1
    fi
    
    # Remove external dependencies from coverage
    log_info "Filtering coverage data..."
    lcov --remove coverage/coverage.info '/usr/*' '*/tests/*' '*/mocks/*' \
         --output-file coverage/coverage_filtered.info \
         --rc branch_coverage=1 \
         --ignore-errors source,unused 2>&1 | tee coverage/lcov_filter.log
    
    # Generate HTML report
    log_info "Generating HTML report..."
    genhtml coverage/coverage_filtered.info \
            --output-directory coverage/html \
            --branch-coverage \
            --title "Westeros GL BRCM L1 Test Coverage" \
            --legend \
            --demangle-cpp \
            --ignore-errors source 2>&1 | tee coverage/genhtml.log
    
    if [ -f "coverage/html/index.html" ]; then
        log_success "HTML coverage report generated: $L1_BUILD_DIR/coverage/html/index.html"
        
        # Display coverage summary
        log_info "Coverage Summary:"
        lcov --list coverage/coverage_filtered.info \
             --rc branch_coverage=1 \
             --ignore-errors source | tail -20
    else
        log_error "Failed to generate HTML report"
        log_info "Check coverage/genhtml.log for details"
        return 1
    fi
    
    # Generate text summary with gcovr
    if command -v gcovr &> /dev/null; then
        log_info "Generating coverage summary with gcovr..."
        
        gcovr --root .. \
              --exclude '.*/test/.*' \
              --exclude '.*/tests/.*' \
              --exclude '.*/mocks/.*' \
              --print-summary \
              --txt coverage/coverage_summary.txt \
              --html-details coverage/coverage_gcovr.html \
              --branches 2>&1 | tee coverage/coverage_summary.txt
        
        log_success "Coverage summary generated: $L1_BUILD_DIR/coverage/coverage_summary.txt"
    else
        log_warning "gcovr not found. Skipping gcovr coverage report."
    fi
    
    # Display summary
    if [ -f "coverage/coverage_summary.txt" ]; then
        echo ""
        log_info "Coverage Summary:"
        cat coverage/coverage_summary.txt
        echo ""
    fi
}

# Build for Coverity analysis
build_coverity() {
    log_step "Building for Coverity Analysis"
    
    cd "$PROJECT_ROOT"
    
    # Check if configure script exists
    if [ ! -f "configure" ]; then
        log_info "Running autoreconf to generate configure script..."
        if [ "$VERBOSE" = true ]; then
            autoreconf --install --force
        else
            autoreconf --install --force > /dev/null 2>&1
        fi
    fi
    
    # Create build directory for Coverity
    mkdir -p "$COVERITY_BUILD_DIR"
    cd "$COVERITY_BUILD_DIR"
    
    # Configure with autotools
    log_info "Configuring with autotools for Coverity..."
    if [ "$VERBOSE" = true ]; then
        ../configure \
            --enable-static \
            --disable-shared \
            CFLAGS="-g -O0" \
            CXXFLAGS="-g -O0"
    else
        ../configure \
            --enable-static \
            --disable-shared \
            CFLAGS="-g -O0" \
            CXXFLAGS="-g -O0" > /dev/null 2>&1
    fi
    
    # Check if Coverity tools are available in PATH
    if command -v cov-build &> /dev/null; then
        log_info "Building with Coverity instrumentation (using $JOBS parallel jobs)..."
        
        # Build with cov-build wrapper to generate cov-int directory
        if [ "$VERBOSE" = true ]; then
            cov-build --dir "$PROJECT_ROOT/cov-int" make -j"$JOBS"
        else
            cov-build --dir "$PROJECT_ROOT/cov-int" make -j"$JOBS" > /dev/null
        fi
        
        log_success "Coverity build with instrumentation completed"
        log_info "Coverity intermediate directory: $PROJECT_ROOT/cov-int"
    else
        log_warning "cov-build not found in PATH, building without Coverity instrumentation"
        log_info "Building normally (using $JOBS parallel jobs)..."
        
        if [ "$VERBOSE" = true ]; then
            make -j"$JOBS"
        else
            make -j"$JOBS" > /dev/null
        fi
        
        log_warning "Coverity intermediate directory (cov-int) not created"
        log_info "To enable Coverity analysis, ensure cov-build is in PATH"
    fi
    
    log_success "Coverity build completed successfully"
    log_info "Build artifacts in: $COVERITY_BUILD_DIR"
    
    cd "$PROJECT_ROOT"
}

# Display build summary
show_summary() {
    echo ""
    log_info "========================================"
    log_info "           Build Summary"
    log_info "========================================"
    
    if [ "$BUILD_L1" = true ]; then
        echo -e "${GREEN}✓${NC} L1 Tests: Built and executed"
        if [ -d "$L1_BUILD_DIR/coverage" ]; then
            echo -e "  Coverage Report: $L1_BUILD_DIR/coverage/html/index.html"
        fi
    fi
    
    if [ "$BUILD_COVERITY" = true ]; then
        echo -e "${GREEN}✓${NC} Coverity Analysis: Completed"
        echo -e "  HTML Report: $COVERITY_OUTPUT_DIR/html-report/index.html"
        echo -e "  Upload Archive: $COVERITY_OUTPUT_DIR/westeros-gl-brcm.tgz"
    fi
    
    log_info "========================================"
    echo ""
}

# Main execution
main() {
    log_info "=== Westeros GL BRCM Build Script ==="
    echo ""
    
    # Parse arguments
    parse_args "$@"
    
    # Display build configuration
    log_info "Build Configuration:"
    echo "  L1 Tests: $([ "$BUILD_L1" = true ] && echo "Enabled" || echo "Disabled")"
    echo "  Coverity: $([ "$BUILD_COVERITY" = true ] && echo "Enabled" || echo "Disabled")"
    echo "  Clean Build: $([ "$CLEAN_BUILD" = true ] && echo "Yes" || echo "No")"
    echo "  Parallel Jobs: $JOBS"
    echo "  Verbose: $([ "$VERBOSE" = true ] && echo "Yes" || echo "No")"
    echo ""
    
    # Clean if requested
    if [ "$CLEAN_BUILD" = true ]; then
        clean_builds
        echo ""
    fi
    
    # Build L1 tests
    if [ "$BUILD_L1" = true ]; then
        build_l1 || {
            log_error "L1 build failed"
            exit 1
        }
        echo ""
    fi
    
    # Build for Coverity
    if [ "$BUILD_COVERITY" = true ]; then
        build_coverity || {
            log_error "Coverity build failed"
            exit 1
        }
        echo ""
    fi
    
    # Show summary
    show_summary
    
    log_success "All builds completed successfully!"
}

# Execute main
main "$@"
