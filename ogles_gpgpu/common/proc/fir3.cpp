//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#include "../common_includes.h"
#include "fir3.h"

using namespace ogles_gpgpu;

// clang-format off
const char *Fir3Proc::fshaderFir3Src =
#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(
 varying vec2 textureCoordinate;

 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 uniform sampler2D inputImageTexture3;

 uniform vec3 weights;
 uniform float alpha;
 uniform float beta;

 void main()
 {
     vec3 textureColor = texture2D(inputImageTexture, textureCoordinate).rgb;
     vec3 textureColor2 = texture2D(inputImageTexture2, textureCoordinate).rgb;
     vec3 textureColor3 = texture2D(inputImageTexture3, textureCoordinate).rgb;
     vec3 response = (textureColor * weights.x) + (textureColor2 * weights.y) + (textureColor3 * weights.z);
     gl_FragColor = vec4(response * alpha + beta, 1.0);
 });

const char *Fir3Proc::fshaderFir3RGBSrc = 
#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(
 varying vec2 textureCoordinate;

 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 uniform sampler2D inputImageTexture3;

 uniform vec3 weights1;
 uniform vec3 weights2;
 uniform vec3 weights3;
 uniform float alpha;
 uniform float beta;

 void main()
 {
     vec3 textureColor = texture2D(inputImageTexture, textureCoordinate).rgb;
     vec3 textureColor2 = texture2D(inputImageTexture2, textureCoordinate).rgb;
     vec3 textureColor3 = texture2D(inputImageTexture3, textureCoordinate).rgb;
     vec3 response = (textureColor * weights1) + (textureColor2 * weights2) + (textureColor3 * weights3);

     gl_FragColor = vec4(response * alpha + beta, 1.0);
});
// clang-format on

Fir3Proc::Fir3Proc(bool doRgb)
    : doRgb(doRgb) {
    Vec3f weights(0.333, 0.333, 0.333);
    setWeights(weights);
    setWeights(weights, weights, weights);

    setAlpha(1.f);
    setBeta(0.f);
}

void Fir3Proc::getUniforms() {
    ThreeInputProc::getUniforms();

    if(doRgb) {
        shParamUWeights1 = shader->getParam(UNIF, "weights1");
        shParamUWeights2 = shader->getParam(UNIF, "weights2");
        shParamUWeights3 = shader->getParam(UNIF, "weights3");
    } else {
        shParamUWeights = shader->getParam(UNIF, "weights");
    }

    shParamUAlpha = shader->getParam(UNIF, "alpha");
    shParamUBeta = shader->getParam(UNIF, "beta");
}

void Fir3Proc::setUniforms() {
    ThreeInputProc::setUniforms();
    if(doRgb) {
        glUniform3fv(shParamUWeights1, 1, &weightsRGB[0].data[0]);
        glUniform3fv(shParamUWeights2, 1, &weightsRGB[1].data[0]);
        glUniform3fv(shParamUWeights3, 1, &weightsRGB[2].data[0]);
    } else {
        glUniform3fv(shParamUWeights, 1, &weights.data[0]);
    }

    glUniform1f(shParamUAlpha, alpha);
    glUniform1f(shParamUBeta, beta);
}
