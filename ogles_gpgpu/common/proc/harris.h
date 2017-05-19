//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

/**
 * GPGPU corner processor.
 */
#ifndef OGLES_GPGPU_COMMON_PROC_HARRIS
#define OGLES_GPGPU_COMMON_PROC_HARRIS

#include "../common_includes.h"

#include "base/filterprocbase.h"

namespace ogles_gpgpu {

/**
 * GPGPU Harris computes corner strength.
 */
class HarrisProc : public FilterProcBase {
public:
    /**
     * Constructor.
     */
    HarrisProc();

    /**
     * Return the processors name.
     */
    virtual const char* getProcName() {
        return "HarrisProc";
    }

    /**
     * Set the sensitivity.
     */
    void setSensitivity(float value) {
        sensitivity = value;
    }

    /**
     * Return the sensitivity.
     */
    float getSensitivity() const {
        return sensitivity;
    }

private:
    /**
     * Set additional uniforms.
     */
    virtual void setUniforms();

    /**
     * Get uniform indices.
     */
    virtual void getUniforms();

    // TODO: we currently need to override the filterShaderSetup to parse uniforms from the GPUImage
    virtual void filterShaderSetup(const char* vShaderSrc, const char* fShaderSrc, GLenum target);

    /**
     * Get the vertex shader source.
     */
    virtual const char* getVertexShaderSource() {
        return vshaderGPUImage;
    }

    /**
     * Get the fragment shader source.
     */
    virtual const char* getFragmentShaderSource() {
        return fshaderHarrisSrc;
    }

    static const char* fshaderHarrisSrc; // fragment shader source

    GLuint shParamUInputSensitivity = 0;

    float sensitivity = 1.5f;
};
}

#endif
