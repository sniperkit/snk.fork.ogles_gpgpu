//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_LOW_PASS_PROC
#define OGLES_GPGPU_COMMON_LOW_PASS_PROC

#include "iir.h"

BEGIN_OGLES_GPGPU

class LowPassFilterProc : public IirFilterProc {
public:
    LowPassFilterProc(float alpha=0.5) : IirFilterProc(IirFilterProc::kLowPass, alpha) {}
    ~LowPassFilterProc() {}
    virtual const char *getProcName() {
        return "LowPassFilterProc";
    }
};

END_OGLES_GPGPU

#endif
