//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

/**
 * GPGPU grad processor.
 */
#ifndef OGLES_GPGPU_COMMON_PROC_NMS
#define OGLES_GPGPU_COMMON_PROC_NMS

#include "ogles_gpgpu/common/proc/filter3x3.h"

namespace ogles_gpgpu {

/**
 * GPGPU non max suppression, gradient magnitude and orientation
 */
class NmsProc : public Filter3x3Proc {
public:
    /**
     * Constructor.
     */
    NmsProc();

    /**
     * Return the processors name.
     */
    virtual const char *getProcName() {
        return "NmsProc";
    }

    /**
     * Set threshold for non maximal suppression
     */
    void setThreshold(float value) {
        threshold = value;
    }

    /**
     * Get threhsold for non maximal suppression
     */
    float getThreshold() const {
        return threshold;
    }

    void swizzle(int channelIn, int channelOut=-1);

private:

    /**
     * Get the fragment shader source.
     */
    virtual const char *getFragmentShaderSource() {
        return fshaderNmsSwizzleSrc.empty() ? fshaderNmsSrc : fshaderNmsSwizzleSrc.c_str();
    }

    /**
     * Get uniform indices.
     */
    virtual void getUniforms();

    /**
     * Set uniforms.
     */
    virtual void setUniforms();

    static const char *fshaderNmsSrc;   // fragment shader source

    std::string fshaderNmsSwizzleSrc;

    GLuint shParamUThreshold;

    float threshold = 0.9;
};
}

#endif
