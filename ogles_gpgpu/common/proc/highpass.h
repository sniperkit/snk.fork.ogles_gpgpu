//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_HIGH_PASS_PROC
#define OGLES_GPGPU_COMMON_HIGH_PASS_PROC

#include "iir.h"

BEGIN_OGLES_GPGPU

class HighPassFilterProc : public IirFilterProc {
public:
    HighPassFilterProc(float alpha = 0.5)
        : IirFilterProc(IirFilterProc::kHighPass, alpha) {
    }
    ~HighPassFilterProc() {}
    virtual const char* getProcName() {
        return "HighPassFilterProc";
    }
};

END_OGLES_GPGPU

#endif
