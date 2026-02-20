#!/bin/bash
# Setup script for Westeros GL BRCM L1 test environment
# Installs dependencies required for building and testing

set -e

echo "=========================================="
echo "Westeros GL BRCM L1 Test Setup"
echo "=========================================="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_status() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
else
    print_error "Cannot detect OS"
    exit 1
fi

echo "Detected OS: $OS $VER"
echo

# Check for sudo
if [ "$EUID" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# Update package list
echo "Updating package list..."
$SUDO apt-get update -qq

# Required packages
PACKAGES=(
    "cmake"
    "build-essential"
    "pkg-config"
    "libgtest-dev"
    "libgmock-dev"
    "libwayland-dev"
    "libegl1-mesa-dev"
    "lcov"
    "gcovr"
    "git"
)

# Check and install packages
echo "Checking dependencies..."
for package in "${PACKAGES[@]}"; do
    if dpkg -l | grep -q "^ii  $package"; then
        print_status "$package is already installed"
    else
        echo "Installing $package..."
        $SUDO apt-get install -y $package
        print_status "$package installed"
    fi
done

echo

# Verify installations
echo "Verifying installations..."

# Check CMake version
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    print_status "CMake version: $CMAKE_VERSION"
    
    # Check if version is >= 3.14
    REQUIRED_VERSION="3.14"
    if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$CMAKE_VERSION" | sort -V | head -n1)" = "$REQUIRED_VERSION" ]; then
        print_status "CMake version is sufficient (>= 3.14)"
    else
        print_error "CMake version must be >= 3.14"
        exit 1
    fi
else
    print_error "CMake not found"
    exit 1
fi

# Check GTest
if [ -f /usr/lib/x86_64-linux-gnu/libgtest.a ] || [ -f /usr/lib/libgtest.a ]; then
    print_status "Google Test found"
else
    print_warning "Google Test may need manual building"
fi

# Check lcov
if command -v lcov &> /dev/null; then
    LCOV_VERSION=$(lcov --version | head -n1 | cut -d' ' -f4)
    print_status "lcov version: $LCOV_VERSION"
else
    print_error "lcov not found"
    exit 1
fi

# Check gcovr
if command -v gcovr &> /dev/null; then
    GCOVR_VERSION=$(gcovr --version | head -n1 | awk '{print $2}')
    print_status "gcovr version: $GCOVR_VERSION"
else
    print_error "gcovr not found"
    exit 1
fi

# Check Wayland
if pkg-config --exists wayland-client; then
    WAYLAND_VERSION=$(pkg-config --modversion wayland-client)
    print_status "Wayland version: $WAYLAND_VERSION"
else
    print_error "Wayland development files not found"
    exit 1
fi

# Check EGL
if pkg-config --exists egl; then
    EGL_VERSION=$(pkg-config --modversion egl)
    print_status "EGL version: $EGL_VERSION"
else
    print_warning "EGL development files not found (using mocks)"
fi

echo
echo "=========================================="
echo "Setup completed successfully!"
echo "=========================================="
echo
echo "Next steps:"
echo "1. cd L1"
echo "2. ./scripts/build.sh"
echo "3. Run tests: ./build/westeros_gl_l1_tests"
echo "4. Generate coverage: make -C build coverage"
echo
