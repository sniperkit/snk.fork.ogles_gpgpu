//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_PROC_DIFF
#define OGLES_GPGPU_COMMON_PROC_DIFF

#include "../common_includes.h"
#include "two.h"

namespace ogles_gpgpu {

// ######### Temporal derivative shader #################

class DiffProc : public TwoInputProc {
public:
    DiffProc(float strength = 1.f, float offset = 0.f);
    ~DiffProc() {}
    virtual const char* getProcName() {
        return "DiffProc";
    }
    virtual void getUniforms();
    virtual void setUniforms();
    virtual void setStrength(float value) {
        strength = value;
    }
    virtual void setOffset(float value) {
        offset = value;
    }
    virtual int render(int position = 0);

private:
    GLuint shParamUStrength;
    GLuint shParamUOffset;
    float strength;
    float offset;
    virtual const char* getVertexShaderSource() {
        return vshaderGPUImage;
    }
    virtual const char* getFragmentShaderSource() {
        return fshaderDiffSrc;
    }
    static const char* fshaderDiffSrc; // fragment shader source (diff)
};
}

#endif // OGLES_GPGPU_COMMON_PROC_DIFF
