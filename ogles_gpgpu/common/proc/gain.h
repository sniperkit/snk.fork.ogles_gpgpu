//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_PROC_GAIN
#define OGLES_GPGPU_COMMON_PROC_GAIN

#include "ogles_gpgpu/common/proc/base/filterprocbase.h"

namespace ogles_gpgpu {

/**
 * GPGPU simple gain filter.
 */
class GainProc : public ogles_gpgpu::FilterProcBase {
public:
    /**
     * Constructor.
     */
    GainProc(float gain=1.f) : gain(gain) {}

    /**
     * Return the processors name.
     */
    virtual const char *getProcName() {
        return "GainProc";
    }

    /**
     * Set the gain coefficient.
     */
    void setGain(float value) {
        gain = value;
    }

private:

    /**
     * Get the fragment shader source.
     */
    virtual const char *getFragmentShaderSource() {
        return fshaderGainSrc;
    }

    /**
     * Get shader uniform id.
     */
    virtual void getUniforms() {
        shParamUGain = shader->getParam(UNIF, "gain");
    }

    /**
     * Set shader uniform values.
     */
    virtual void setUniforms() {
        glUniform1f(shParamUGain, gain);
    }

    static const char *fshaderGainSrc; // fragment shader source
    float gain = 1.f;
    GLint shParamUGain;
};

}

#endif

