//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#include "lbp.h"
#include "../common_includes.h"

using namespace std;
using namespace ogles_gpgpu;

// clang-format off
const char *LbpProc::fshaderLbpSrc =

#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(
varying vec2 textureCoordinate;
varying vec2 leftTextureCoordinate;
varying vec2 rightTextureCoordinate;

varying vec2 topTextureCoordinate;
varying vec2 topLeftTextureCoordinate;
varying vec2 topRightTextureCoordinate;

varying vec2 bottomTextureCoordinate;
varying vec2 bottomLeftTextureCoordinate;
varying vec2 bottomRightTextureCoordinate;

uniform sampler2D inputImageTexture;

void main()
{
    float centerIntensity = texture2D(inputImageTexture, textureCoordinate).r;
    float bottomLeftIntensity = texture2D(inputImageTexture, bottomLeftTextureCoordinate).r;
    float topRightIntensity = texture2D(inputImageTexture, topRightTextureCoordinate).r;
    float topLeftIntensity = texture2D(inputImageTexture, topLeftTextureCoordinate).r;
    float bottomRightIntensity = texture2D(inputImageTexture, bottomRightTextureCoordinate).r;
    float leftIntensity = texture2D(inputImageTexture, leftTextureCoordinate).r;
    float rightIntensity = texture2D(inputImageTexture, rightTextureCoordinate).r;
    float bottomIntensity = texture2D(inputImageTexture, bottomTextureCoordinate).r;
    float topIntensity = texture2D(inputImageTexture, topTextureCoordinate).r;

    float byteTally = 1.0 / 255.0 * step(centerIntensity, topRightIntensity);
   byteTally += 2.0 / 255.0 * step(centerIntensity, topIntensity);
   byteTally += 4.0 / 255.0 * step(centerIntensity, topLeftIntensity);
   byteTally += 8.0 / 255.0 * step(centerIntensity, leftIntensity);
   byteTally += 16.0 / 255.0 * step(centerIntensity, bottomLeftIntensity);
   byteTally += 32.0 / 255.0 * step(centerIntensity, bottomIntensity);
   byteTally += 64.0 / 255.0 * step(centerIntensity, bottomRightIntensity);
   byteTally += 128.0 / 255.0 * step(centerIntensity, rightIntensity);

   // TODO: Replace the above with a dot product and two vec4s
   // TODO: Apply step to a matrix, rather than individually

    //gl_FragColor = vec4(centerIntensity, centerIntensity, centerIntensity, 1.0);
    gl_FragColor = vec4(byteTally, byteTally, byteTally, 1.0);
});
// clang-format on

LbpProc::LbpProc() {
}

void LbpProc::getUniforms() {
    Filter3x3Proc::getUniforms();
    shParamUInputTex = shader->getParam(UNIF, "inputImageTexture");
}
