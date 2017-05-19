//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

// NOTE: This will require higher precision via float bit packing to be truly useful

#include "remap.h"

using namespace ogles_gpgpu;

// clang-format off
const char *RemapProc::vshaderRemapSrc = OG_TO_STR
(
    attribute vec4 position;
    attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;
    void main()
    {
        gl_Position = position;
        textureCoordinate = inputTextureCoordinate.xy;
    }
);
// clang-format on

// clang-format off
const char *RemapProc::fshaderRemapSrc = 
#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision highp float;)
#endif
OG_TO_STR(    
 varying vec2 textureCoordinate;
 uniform sampler2D inputImageTexture;  // IMAGE
 uniform sampler2D inputImageTexture2; // FLOW

 uniform float texelWidth;
 uniform float texelHeight;

 void main()
 {
     vec2 offset = vec2(texelWidth, -texelHeight);
     vec2 delta = texture2D(inputImageTexture2, textureCoordinate).xy * 2.0 - 1.0;
     vec4 centerIntensity = texture2D(inputImageTexture, textureCoordinate + (delta * offset));
     gl_FragColor = centerIntensity;
 });
// clang-format on

RemapProc::RemapProc() {}

void RemapProc::filterShaderSetup(const char *vShaderSrc, const char *fShaderSrc, GLenum target) {
    // create shader object
    ProcBase::createShader(vShaderSrc, fShaderSrc, target);

    // get shader params
    shParamAPos = shader->getParam(ATTR, "position");
    shParamATexCoord = shader->getParam(ATTR, "inputTextureCoordinate");
    texelWidthUniform = shader->getParam(UNIF, "texelWidth");
    texelHeightUniform = shader->getParam(UNIF, "texelHeight");

    Tools::checkGLErr(getProcName(), "filterShaderSetup");
}

void RemapProc::getUniforms() {

}

void RemapProc::setUniforms() {
    // Set texel width/height uniforms:
    glUniform1f(texelWidthUniform, (1.0f/ float(outFrameW)));
    glUniform1f(texelHeightUniform, (1.0f/ float(outFrameH)));
}

