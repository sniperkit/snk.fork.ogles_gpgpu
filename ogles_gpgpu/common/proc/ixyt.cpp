//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#include "../common_includes.h"
#include "ixyt.h"

using namespace ogles_gpgpu;

void IxytProc::getUniforms() {
    TwoInputProc::getUniforms();
    texelWidthUniform = shader->getParam(UNIF, "texelWidth");
    texelHeightUniform = shader->getParam(UNIF, "texelHeight");
    shParamUStrength = shader->getParam(UNIF, "strength");
}

void IxytProc::setUniforms() {
    TwoInputProc::setUniforms();
    glUniform1f(texelWidthUniform, (1.0f/ float(outFrameW)));
    glUniform1f(texelHeightUniform, (1.0f/ float(outFrameH)));
    glUniform1f(shParamUStrength, strength);
}

// *INDENT-OFF*
const char *IxytProc::fshaderIxytSrc =
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
 uniform sampler2D inputImageTexture2;

 uniform float strength;

 void main()
 {
     float bottomLeftIntensity = texture2D(inputImageTexture, bottomLeftTextureCoordinate).r;
     float topRightIntensity = texture2D(inputImageTexture, topRightTextureCoordinate).r;
     float topLeftIntensity = texture2D(inputImageTexture, topLeftTextureCoordinate).r;
     float bottomRightIntensity = texture2D(inputImageTexture, bottomRightTextureCoordinate).r;
     float centerIntensity = texture2D(inputImageTexture, textureCoordinate).r;
     float leftIntensity = texture2D(inputImageTexture, leftTextureCoordinate).r;
     float rightIntensity = texture2D(inputImageTexture, rightTextureCoordinate).r;
     float bottomIntensity = texture2D(inputImageTexture, bottomTextureCoordinate).r;
     float topIntensity = texture2D(inputImageTexture, topTextureCoordinate).r;

     float y = -topLeftIntensity - (2.0 * topIntensity) - topRightIntensity + bottomLeftIntensity + (2.0 * bottomIntensity) + bottomRightIntensity;
     float x = -bottomLeftIntensity - (2.0 * leftIntensity) - topLeftIntensity + bottomRightIntensity + (2.0 * rightIntensity) + topRightIntensity;
     y = y / 8.0;
     x = x / 8.0;

     //float x = (rightIntensity - leftIntensity) / 2.0;
     //float y = (bottomIntensity - topIntensity) / 2.0;

     float centerIntensity2 = texture2D(inputImageTexture2, textureCoordinate).r;
     float t = (centerIntensity-centerIntensity2) / 2.0; // TODO: smooth dt

     vec3 d = vec3(x, y, t) * strength;
     vec3 d2 = (d + 1.0) / 2.0;

     gl_FragColor = vec4(d2, centerIntensity);
 }
 );
// *INDENT-ON*
