//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Original: http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
// Modified: http://stackoverflow.com/a/17897228
// Modified: Copyright (c) 2017, David Hirvonen (this file)

#include "rgb2hsv.h"

// *INDENT-OFF*
BEGIN_OGLES_GPGPU
const char * Rgb2HsvProc::fshaderRgb2HsvSrc = 
#if defined(OGLES_GPGPU_OPENGLES)
OG_TO_STR(precision mediump float;)
#endif
OG_TO_STR(
 vec3 rgb2hsv(vec3 c)
 {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
 }

 varying vec2 vTexCoord;
 uniform sampler2D uInputTex;
 void main()
 {
     vec4 val = texture2D(uInputTex, vTexCoord);
     gl_FragColor = vec4(rgb2hsv(val.rgb), 1.0);
 });

END_OGLES_GPGPU
// *INDENT-ON*
