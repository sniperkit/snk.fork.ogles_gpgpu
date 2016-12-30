//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015 (http://www.mkonrad.net)
//         David Hirvonen
//
// See LICENSE file in project repository root for the license.
//

#include "../common_includes.h"
#include "hessian.h"

//>> fx=[-1 1; -1 1]; fy=[-1 -1; 1 1];
//
//>> conv2(fx, fx)
//
// ans =
//
//  1    -2     1
//  2    -4     2
//  1    -2     1
//
// >> conv2(fx, fy)
//
// ans =
//
//  1     0    -1
//  0     0     0
// -1     0     1
//
// >> conv2(fy, fy)
//
// ans =
//
//  1     2     1
// -2    -4    -2
//  1     2     1

using namespace std;
using namespace ogles_gpgpu;

// Source: GPUImageXYDerivativeFilter.m
const char *HessianProc::fshaderHessianSrc = OG_TO_STR(

#if defined(OGLES_GPGPU_OPENGLES)
precision highp float;
#endif

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

uniform float edgeStrength;

void main()
{
    float topIntensity = texture2D(inputImageTexture, topTextureCoordinate).r;
    float topRightIntensity = texture2D(inputImageTexture, topRightTextureCoordinate).r;
    float topLeftIntensity = texture2D(inputImageTexture, topLeftTextureCoordinate).r;
    float bottomIntensity = texture2D(inputImageTexture, bottomTextureCoordinate).r;
    float centerIntensity = texture2D(inputImageTexture, textureCoordinate).r;
    float bottomLeftIntensity = texture2D(inputImageTexture, bottomLeftTextureCoordinate).r;
    float bottomRightIntensity = texture2D(inputImageTexture, bottomRightTextureCoordinate).r;
    float leftIntensity = texture2D(inputImageTexture, leftTextureCoordinate).r;
    float rightIntensity = texture2D(inputImageTexture, rightTextureCoordinate).r;
    
    //  1     2     1
    // -2    -4    -2
    //  1     2     1
    float Iyy0 = topLeftIntensity + 2.0*topIntensity + topRightIntensity;
    float Iyy1 = (leftIntensity + 2.0*centerIntensity + rightIntensity) * 2.0;
    float Iyy2 = bottomLeftIntensity + 2.0*bottomIntensity + bottomRightIntensity;
    float Iyy = (Iyy0 - Iyy1 + Iyy2) / 16.0;

    // 1    -2     1
    // 2    -4     2
    // 1    -2     1
    float Ixx0 = topLeftIntensity + 2.0*leftIntensity + bottomLeftIntensity;
    float Ixx1 = (topIntensity  + 2.0*centerIntensity + bottomIntensity) * 2.0;
    float Ixx2 = topRightIntensity + 2.0*rightIntensity + bottomRightIntensity;
    float Ixx = (Ixx0 - Ixx1 + Ixx2) / 16.0;
    
    //  1     0    -1
    //  0     0     0
    // -1     0     1
    float Ixy = (topLeftIntensity + bottomRightIntensity - topRightIntensity - bottomLeftIntensity) / 4.0;
    
    // Compute determinant of Hessian:
    //float d = ((Ixx + Iyy) < 0.0) ? ((Ixx * Iyy) - (Ixy * Ixy)) : 0.0;
    float d = (Ixx * Iyy) - (Ixy * Ixy);
    gl_FragColor = vec4((Ixx + 1.0)/2.0, (Iyy + 1.0)/2.0, (Ixy + 1.0)/2.0, d * edgeStrength);
});

HessianProc::HessianProc(float edgeStrength) : edgeStrength(edgeStrength) {

}

void HessianProc::setUniforms() {
    Filter3x3Proc::setUniforms();
    glUniform1f(shParamUEdgeStrength, edgeStrength);
}

void HessianProc::getUniforms() {
    Filter3x3Proc::getUniforms();
    shParamUInputTex = shader->getParam(UNIF, "inputImageTexture");
    shParamUEdgeStrength = shader->getParam(UNIF, "edgeStrength");
}

