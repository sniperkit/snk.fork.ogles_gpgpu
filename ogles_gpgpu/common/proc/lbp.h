//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

/**
 * GPGPU lbp processor.
 */
#ifndef OGLES_GPGPU_COMMON_PROC_LBP
#define OGLES_GPGPU_COMMON_PROC_LBP

#include "ogles_gpgpu/common/proc/filter3x3.h"

namespace ogles_gpgpu {

/**
 * GPGPU lbpient, lbpient magnitude and orientation
 */
class LbpProc : public Filter3x3Proc {
public:
    /**
     * Constructor.
     */
    LbpProc();

    /**
     * Return the processors name.
     */
    virtual const char* getProcName() {
        return "LbpProc";
    }

private:
    /**
     * Get the fragment shader source.
     */
    virtual const char* getFragmentShaderSource() {
        return fshaderLbpSrc;
    }

    /**
     * Get uniform indices.
     */
    virtual void getUniforms();

    static const char* fshaderLbpSrc; // fragment shader source
};
}

#endif
