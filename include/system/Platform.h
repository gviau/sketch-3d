#ifndef SKETCH_3D_PLATFORM_H
#define SKETCH_3D_PLATFORM_H

#include <string>
using namespace std;

// This file is heavely inspired from Ogre3D's OgrePlatform.h file and
// OgrePlatformInformation.h files. Big thanks to them.
namespace Sketch3D
{
#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2

#define COMPILER_MSVC 1
#define COMPILER_GNUC 2

#define ARCHITECTURE_32 1
#define ARCHITECTURE_64 2

#define CPU_X86 1

// Compiler
#if defined(_MSC_VER)
#   define COMPILER COMPILER_MSVC
#elif defined(__GNUC__)
#   define COMPILER COMPILER_GNUC
#else
#   pragma error "Unsupported compiler!"
#endif

// Platform
#if defined(__WIN32__) || defined(_WIN32)
#   define PLATFORM PLATFORM_WIN32
#else
#   define PLATFORM PLATFORM_LINUX
#endif

// Architecture (32 bits or 64 bits?)
#if defined(__x86_64__) || defined(_M_X64)
#   define ARCH_TYPE ARCHITECTURE_64
#else
#   define ARCH_TYPE ARCHITECTURE_32
#endif

// Cpu type
#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
#   define CPU CPU_X86
#endif

// How to declare aligned variable
#if COMPILER == COMPILER_MSVC
#   define ALIGNED_DECL(type, var, alignment) __declspec(align(alignment)) type var
#elif COMPILER == COMPILER_GNUC
#   define ALIGNED_DECL(type, var, alignment) type var __atribute__((aligned(alignment)))
#else
#   define ALIGNED_DECL(type, var, alignment) type var
#endif

// SIMD stuff
#define SIMD_ALIGNMENT 16
#define SIMD_ALIGNED_DECL(type, var) ALIGNED_DECL(type, var, SIMD_ALIGNMENT)

#if CPU == CPU_X86 && COMPILER == COMPILER_MSVC
#   define HAVE_SSE 1
#elif CPU == CPU_X86 && COMPILER == COMPILER_GNUC
#   define HAVE_SSE 1
#else
#   define HAVE_SSE 0
#endif

#if HAVE_SSE == 1
#   include <emmintrin.h>
#endif

/**
 * @class PlatformInformation
 * This class provides methods to retrieve information about the cpu.
 */
class PlatformInformation
{
    public:
        enum CpuFeatures {
            SSE         = 1 << 0,
            SSE2        = 1 << 1,
            MMX         = 1 << 2
        };

        /**
         * Gets a string of the CPU identifier
         */
        static const string&    GetCpuIdentifier();

        /**
         * Returns the features supported by the cpu
         */
        static unsigned int     GetCpuFeatures();

        /**
         * Tests wether or not a feature is supported by the cpu
         * @param feature The feature to test
         * @return true if the feature is supported, false otherwise
         */
        static bool             HasCpuFeature(CpuFeatures feature);
};
}

#endif
