//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_RGB2HSV_PROC
#define OGLES_GPGPU_COMMON_RGB2HSV_PROC

#include "ogles_gpgpu/common/proc/base/filterprocbase.h"

BEGIN_OGLES_GPGPU

class Rgb2HsvProc : public ogles_gpgpu::FilterProcBase {
public:
    Rgb2HsvProc(float gain=1.f) : gain(gain) {}
    virtual const char *getProcName() {
        return "Rgb2HsvProc";
    }
private:
    virtual const char *getFragmentShaderSource() {
        return fshaderRgb2HsvSrc;
    }
    virtual void getUniforms() {
        shParamUGain = shader->getParam(UNIF, "gain");
    }
    virtual void setUniforms() {
        glUniform1f(shParamUGain, gain);
    }
    static const char *fshaderRgb2HsvSrc; // fragment shader source
    float gain = 1.f;
    GLint shParamUGain;
};

END_OGLES_GPGPU

#endif
