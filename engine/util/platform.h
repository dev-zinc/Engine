#pragma once

namespace Platform {

#if defined(__APPLE__)
    constexpr bool isMac = true;
    constexpr bool isWindows = false;
#elif defined(_WIN32)
    constexpr bool isMac = false;
    constexpr bool isWindows = true;
#else
    #error "Unknown platform"
#endif

}
