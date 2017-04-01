//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Original shader: http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// Modified source: https://github.com/hughsk/glsl-hsv2rgb
// Formatting: Copyright (c) 2017, David Hirvonen (this file)

#include "hsv2rgb.h"

BEGIN_OGLES_GPGPU
// *INDENT-OFF*
const char * Hsv2RgbProc::fshaderHsv2RgbSrc = OG_TO_STR
(
#if defined(OGLES_GPGPU_OPENGLES)
 precision mediump float;
#endif

  vec3 hsv2rgb(vec3 c)
  {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
  }

 varying vec2 vTexCoord;
 uniform sampler2D uInputTex;
 uniform float gain;
 void main()
 {
     vec4 val = texture2D(uInputTex, vTexCoord);
     gl_FragColor = vec4(hsv2rgb(val.rgb), 1.0);
 });
// *INDENT-ON*
END_OGLES_GPGPU
