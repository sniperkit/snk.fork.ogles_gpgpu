//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2017, David Hirvonen (this file)

#include "gain.h"

// *INDENT-OFF*
BEGIN_OGLES_GPGPU
const char * GainProc::fshaderGainSrc = OG_TO_STR
(
#if defined(OGLES_GPGPU_OPENGLES)
 precision mediump float;
#endif
 varying vec2 vTexCoord;
 uniform sampler2D uInputTex;
 uniform float gain;
 void main()
 {
     vec4 val = texture2D(uInputTex, vTexCoord);
     gl_FragColor = clamp(val * gain, 0.0, 1.0);
 });
END_OGLES_GPGPU
// *INDENT-ON*
