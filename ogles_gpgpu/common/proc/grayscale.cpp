//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

// Modifications: Copyright (c) 2016-2017, David Hirvonen (this file)

#include "grayscale.h"
#include "../common_includes.h"

#include <memory.h>

using namespace std;
using namespace ogles_gpgpu;

const GLfloat GrayscaleProc::grayscaleConvVecRGB[3] = {
    0.299, 0.587, 0.114
};

const GLfloat GrayscaleProc::grayscaleConvVecBGR[3] = {
    0.114, 0.587, 0.299
};

const GLfloat GrayscaleProc::grayscaleConvVecNone[3] = {
    1.0, 1.0, 1.0
};

// clang-format off
const char *GrayscaleProc::fshaderGrayscaleSrc = 

#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision mediump float;)
#endif

OG_TO_STR(
uniform sampler2D uInputTex;
uniform vec3 uInputConvVec;
varying vec2 vTexCoord;

void main()
{
    float gray = dot(texture2D(uInputTex, vTexCoord).rgb, uInputConvVec);
    gl_FragColor = vec4(gray, gray, gray, 1.0);
}
);
// clang-format on

// clang-format off
const char *GrayscaleProc::fshaderNoopSrc =

#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision mediump float;)
#endif

OG_TO_STR(
 varying vec2 vTexCoord;
 uniform sampler2D uInputTex;
 void main()
 {
     gl_FragColor = vec4(texture2D(uInputTex, vTexCoord).rgba);
 }
);
// clang-format on

GrayscaleProc::GrayscaleProc() {
    // set defaults
    inputConvType = GRAYSCALE_INPUT_CONVERSION_NONE;
    setGrayscaleConvType(GRAYSCALE_INPUT_CONVERSION_RGB);
}

void GrayscaleProc::setIdentity() {
}

void GrayscaleProc::setUniforms() {
    if (inputConvType != GRAYSCALE_INPUT_CONVERSION_NONE) {
        glUniform3fv(shParamUInputConvVec, 1, grayscaleConvVec); // set additional uniforms
    }
}

void GrayscaleProc::getUniforms() {
    if (inputConvType != GRAYSCALE_INPUT_CONVERSION_NONE) {
        shParamUInputConvVec = shader->getParam(UNIF, "uInputConvVec");
    }
}

void GrayscaleProc::setGrayscaleConvVec(const GLfloat v[3]) {
    inputConvType = GRAYSCALE_INPUT_CONVERSION_CUSTOM;
    memcpy(grayscaleConvVec, v, sizeof(GLfloat) * 3);
}

void GrayscaleProc::setGrayscaleConvType(GrayscaleInputConversionType type) {
    if (inputConvType == type)
        return; // no change

    const GLfloat* v = NULL;

    switch (type) {
    case GRAYSCALE_INPUT_CONVERSION_RGB:
        v = &grayscaleConvVecRGB[0];
        break;
    case GRAYSCALE_INPUT_CONVERSION_BGR:
        v = &grayscaleConvVecBGR[0];
        break;
    case GRAYSCALE_INPUT_CONVERSION_NONE:
        v = &grayscaleConvVecNone[0];
        break;
    default:
        v = &grayscaleConvVecNone[0];
        OG_LOGERR(getProcName(), "unknown grayscale input conversion type %d", type);
    }

    memcpy(grayscaleConvVec, v, sizeof(GLfloat) * 3);

    inputConvType = type;
}
