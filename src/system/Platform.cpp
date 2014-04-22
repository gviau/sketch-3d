#include "system/Platform.h"

#include "system/Logger.h"

#include <cstring>
#include <iostream>

namespace Sketch3D
{
#if COMPILER == COMPILER_MSVC
#   include <excpt.h>
#   if _MSC_VER >= 1400
#       include <intrin.h>
#   endif
#elif COMPILER == COMPILER_GNUC
#   include <signal.h>
#   include <setjmp.h>
#endif

#if CPU == CPU_X86
    struct CpuidResult
    {
        unsigned int _eax;
        unsigned int _ebx;
        unsigned int _edx;
        unsigned int _ecx;
    };

#if COMPILER == COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4035) // no return value
#endif

    static int SupportCpuid()
    {
#if COMPILER == COMPILER_MSVC
#   if _MSC_VER >= 1400 && defined(_M_X64)
        return true;
#   else
        __asm
        {
            // Read Eflag
            pushfd
            pop     eax
            mov     ecx, eax

            // Modify bit 21
            xor     eax, 0x200000
            push    eax
            popfd

            // Read back Eflag
            pushfd
            pop     eax

            // Restore Eflag
            push    ecx
            popfd

            // Check but 21 modifiable
            xor     eax, ecx
            neg     eax
            sbb     eax, eax
        }
#   endif
#elif COMPILER == COMPILER_GNUC
#   if ARCH_TYPE == ARCHITECTURE_64
        return true;
#   else
        unsigned oldFlags, newFlags;
        __asm__
        (
            "pushfl         \n\t"
            "pop    %0      \n\t"
            "mov    %0, %1  \n\t"
            "xor    %2, %0  \n\t"
            "push   %0      \n\t"
            "popfl          \n\t"
            "pushfl         \n\t"
            "pop    %0      \n\t"
            "push   %1      \n\t"
            "popfl          \n\t"
            : "=r" (oldFlags), "=r" (newFlags)
            : "n" (0x200000)
        );

        return oldFlags != newFlags;
#   endif
#else
        return false;
#endif
    }

    static unsigned int PerformCpuid(int query, CpuidResult& result)
    {
#if COMPILER == COMPILER_MSVC
#   if _MSC_VER >= 1400
        int CPUInfo[4];
        __cpuid(CPUInfo, query);
        result._eax = CPUInfo[0];
        result._ebx = CPUInfo[1];
        result._ecx = CPUInfo[2];
        result._edx = CPUInfo[3];
        return result._eax;

#   else
        __asm
        {
            mov     edi, result
            mov     eax, query
            cpuid
            mov     [edi]._eax, eax
            mov     [edi]._ebx, ebx
            mov     [edi]._edx, edx
            mov     [edi]._ecx, ecx
        }
#   endif
#elif COMPILER == COMPILER_GNUC
#   if ARCH_TYPE == ARCHITECTURE_64
        __asm__
        (
            "cpuid" : "=a" (result._eax), "=b" (result._ebx), "=c" (result._ecx), "=d" (result._edx) : "a" (query)
        );
#   else
        __asm__
        (
            "pushl  %%ebx       \n\t"
            "cpuid              \n\t"
            "movl   %%ebx, %%edi\n\t"
            "popl   %%ebx       \n\t"
            : "=a" (result._eax), "=D" (result._ebx), "=c" (result._ecx), "=d" (result._edx)
            : "a" (query)
        );
#   endif
        return result._eax;
#endif
        return 0;
    }

#if COMPILER == COMPILER_MSVC
#pragma warning(pop)
#endif

// Detect SIMD
#if COMPILER == COMPILER_GNUC
#   if ARCH_TYPE == ARCHITECTURE_32
    static jmp_buf sIllegalJmpBuf;
    static void IllegalHandler(int x)
    {
        (void)(x);
        longjmp(sIllegalJmpBuf, 1);
    }
#   endif
#endif

    static bool CheckOperatingSystemSupportSSE()
    {
#if COMPILER == COMPILER_MSVC
#   if _MSC_VER >= 1400 && defined(_M_X64)
        return true;
#   else
        __try
        {
            __asm orps xmm0, xmm0
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
#   endif
#elif COMPILER == COMPILER_GNUC
#   if ARCH_TYPE == ARCHITECTURE_64
        return true;
#   else
        void (*oldHandler)(int);
        oldHandler = signal(SIGILL, IllegalHandler);

        if (setjmp(sIllegalJpmpBuf)) {
            signal(SIGILL, oldHandler);
            return false;
        } else {
            __asm__ __volatile__ ("orps %xmm0, %xmm0");
            signal(SIGILL, oldHandler);
            return true;
        }
#   endif
#else
        return true;
#endif
    }

    static unsigned int QueryCpuFeatures()
    {
#define CPUID_STD_MMX       (1 << 23)
#define CPUID_STD_SSE       (1 << 25)
#define CPUID_STD_SSE2      (1 << 26)

        unsigned int features = 0;
        if (SupportCpuid()) {
            CpuidResult result;

            if (PerformCpuid(0, result)) {
				bool continueFeatures = false;
                if (memcmp(&result._ebx, "GenuineIntel", 12) == 0) {
					continueFeatures = true;
					Logger::getInstance()->info("CPU manufacturer: GenuineIntel");

                } else if (memcmp(&result._ebx, "AuthenticAMD", 12) == 0) {
					continueFeatures = true;
					Logger::getInstance()->info("CPU manufacturer: AuthenticAMD");

                } else {
					Logger::getInstance()->warning("CPU manufacturer: Unknown");
				}

				if (continueFeatures) {
					Logger::getInstance()->info("CPU features:");
                    PerformCpuid(1, result);

                    if (result._edx & CPUID_STD_MMX) {
                        features |= PlatformInformation::MMX;
						Logger::getInstance()->info("MMX supported");
                    }

                    if (result._edx & CPUID_STD_SSE) {
                        features |= PlatformInformation::SSE;
						Logger::getInstance()->info("SSE supported");
                    }

                    if (result._edx & CPUID_STD_SSE2) {
                        features |= PlatformInformation::SSE2;
						Logger::getInstance()->info("SSE2 supported");
                    }
				}
            }
        } else {
			Logger::getInstance()->warning("Couldn't perform CPUID operation");
		}

        return features;
    }

    static unsigned int DetectCpuFeatures()
    {
		Logger::getInstance()->info("Querying CPU info...");

        unsigned int features = QueryCpuFeatures();
        const unsigned int sse_features = PlatformInformation::SSE |
            PlatformInformation::SSE2;
        if ((features & sse_features) && !CheckOperatingSystemSupportSSE()) {
            features &= ~sse_features;
        }

        return features;
    }

    static string DetectCpuIdentifier()
    {
        string cpuId;

        return cpuId;
    }

    const string& PlatformInformation::GetCpuIdentifier()
    {
        static const string identifier = DetectCpuIdentifier();
        return identifier;
    }

    unsigned int PlatformInformation::GetCpuFeatures()
    {
        static const unsigned int features = DetectCpuFeatures();
        return features;
    }

    bool PlatformInformation::HasCpuFeature(CpuFeatures feature)
    {
        return (GetCpuFeatures() & feature) != 0;
    }

#endif
}
