//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_BLEND_PROC
#define OGLES_GPGPU_COMMON_BLEND_PROC

#include "../common_includes.h"
#include "two.h"

BEGIN_OGLES_GPGPU

class BlendProc : public TwoInputProc {
public:
    BlendProc(float alpha = 1.0f);
    virtual const char *getProcName() {
        return "BlendProc";
    }
    virtual void getUniforms();
    virtual void setUniforms();
    virtual void setAlpha(float value) {
        alpha = value;
    }
    virtual int render(int position = 0);

private:

    GLint shParamUAlpha;
    GLfloat alpha = 0.5f;

    virtual const char *getFragmentShaderSource() {
        return fshaderBlendSrc;
    }
    static const char *fshaderBlendSrc;   // fragment shader source
};

END_OGLES_GPGPU

#endif // OGLES_GPGPU_COMMON_PROC_IXYT
