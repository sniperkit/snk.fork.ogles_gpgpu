//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

/**
 * GPGPU tensor processor.
 */
#ifndef OGLES_GPGPU_COMMON_PROC_TENSOR
#define OGLES_GPGPU_COMMON_PROC_TENSOR

#include "ogles_gpgpu/common/proc/filter3x3.h"

namespace ogles_gpgpu {

/**
 * GPGPU gradient, gradient magnitude and orientation
 */
class TensorProc : public Filter3x3Proc {
public:
    /**
     * Constructor.
     */
    TensorProc();

    /**
     * Return the processors name.
     */
    virtual const char *getProcName() {
        return "TensorProc";
    }

    void setEdgeStrength(float strength) {
        edgeStrength = strength;
    }

    float getEdgeStrength() const {
        return edgeStrength;
    }

private:

    /**
     * Get the fragment shader source.
     */
    virtual const char *getFragmentShaderSource() {
        return fshaderTensorSrc;
    }

    /**
     * Set uniform values;
     */
    virtual void setUniforms();

    /**
     * Get uniform indices.
     */
    virtual void getUniforms();

    float edgeStrength = 1.0f;

    GLuint shParamUEdgeStrength = 0;

    static const char *fshaderTensorSrc;   // fragment shader source
};
}

#endif
