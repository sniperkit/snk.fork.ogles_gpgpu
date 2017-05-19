//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

#ifndef OGLES_GPGPU_COMMON_MACROS
#define OGLES_GPGPU_COMMON_MACROS

#define OG_TO_STR_(x) #x
#define OG_TO_STR(x) OG_TO_STR_(x)

#if defined(OGLES_GPGPU_VERBOSE)
#define OG_LOGINF(class_tag, fmt, ...)                                                                                      \
    do {                                                                                                                    \
        fprintf(stderr, "ogles_gpgpu::%s - %s:%d:%s(): " fmt "\n", class_tag, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define OG_LOGINF(class_tag, fmt, ...)
#endif

#define OG_LOGERR(class_tag, fmt, ...)                                                                                      \
    do {                                                                                                                    \
        fprintf(stderr, "ogles_gpgpu::%s - %s:%d:%s(): " fmt "\n", class_tag, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

#endif
