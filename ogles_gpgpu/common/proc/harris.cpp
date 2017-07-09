//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#include "harris.h"
#include "../common_includes.h"

using namespace std;
using namespace ogles_gpgpu;

// Try Harris:

// clang-format off
const char *HarrisProc::fshaderHarrisSrc = 

#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(
varying vec2 textureCoordinate;

uniform sampler2D inputImageTexture;
uniform float sensitivity;

const float harrisConstant = 0.04;

void main()
{
    vec3 derivativeElements = texture2D(inputImageTexture, textureCoordinate).rgb;
    float derivativeSum = derivativeElements.x + derivativeElements.y;
    float zElement = (derivativeElements.z * 2.0) - 1.0;

    // R = Ix^2 * Iy^2 - Ixy * Ixy - k * (Ix^2 + Iy^2)^2
    float cornerness = derivativeElements.x * derivativeElements.y - (zElement * zElement) - harrisConstant * derivativeSum * derivativeSum;

    gl_FragColor = vec4(vec3(cornerness * sensitivity), 1.0);
});
// clang-format on

HarrisProc::HarrisProc() {
}

// TODO: We need to override this if we are using the GPUImage shaders
void HarrisProc::filterShaderSetup(const char* vShaderSrc, const char* fShaderSrc, GLenum target) {
    // create shader object
    ProcBase::createShader(vShaderSrc, fShaderSrc, target);

    // get shader params
    shParamAPos = shader->getParam(ATTR, "position");
    shParamATexCoord = shader->getParam(ATTR, "inputTextureCoordinate");
    Tools::checkGLErr(getProcName(), "filterShaderSetup");
}

void HarrisProc::getUniforms() {
    FilterProcBase::getUniforms();
    shParamUInputTex = shader->getParam(UNIF, "inputImageTexture");
    shParamUInputSensitivity = shader->getParam(UNIF, "sensitivity");
}

void HarrisProc::setUniforms() {
    FilterProcBase::setUniforms();
    glUniform1f(shParamUInputSensitivity, sensitivity); // set additional uniforms
}
