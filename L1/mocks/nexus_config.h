/**
 * Mock Nexus configuration header for L1 testing
 * Provides minimal definitions needed for compilation without actual Nexus platform
 */
#ifndef NEXUS_CONFIG_H
#define NEXUS_CONFIG_H

// Platform version for compatibility checks
#define NEXUS_PLATFORM_VERSION_MAJOR 18
#define NEXUS_PLATFORM_VERSION_MINOR 0

// Endianness configuration
#ifndef BIG_ENDIAN_CPU
// Default to little-endian for most test platforms
#undef BIG_ENDIAN_CPU
#endif

// Platform capabilities
#define NEXUS_HAS_GRAPHICS2D 1
#define NEXUS_HAS_SURFACE 1

#endif // NEXUS_CONFIG_H
