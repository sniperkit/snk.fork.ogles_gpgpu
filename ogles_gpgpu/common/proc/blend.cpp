//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#include "blend.h"
#include "../common_includes.h"

using namespace ogles_gpgpu;

// clang-format off
const char *BlendProc::fshaderBlendSrc =
#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(
 varying vec2 textureCoordinate;
 varying vec2 textureCoordinate2;

 uniform sampler2D inputImageTexture;
 uniform sampler2D inputImageTexture2;
 uniform float alpha;

 void main()
 {
    vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
    vec4 textureColor2 = texture2D(inputImageTexture2, textureCoordinate);
    gl_FragColor = mix(textureColor, textureColor2, alpha);
 });
// clang-format on

BlendProc::BlendProc(float alpha)
    : alpha(alpha) {
}

void BlendProc::getUniforms() {
    TwoInputProc::getUniforms();
    shParamUAlpha = shader->getParam(UNIF, "alpha");
}

void BlendProc::setUniforms() {
    TwoInputProc::setUniforms();
    glUniform1f(shParamUAlpha, alpha);
}

int BlendProc::render(int position) {
    // Always render on position == 0
    TwoInputProc::render(position);
    return position;
}
