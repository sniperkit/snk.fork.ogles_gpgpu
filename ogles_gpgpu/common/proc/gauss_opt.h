//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

/**
 * GPGPU gaussian smoothing processor (two-pass).
 */
#ifndef OGLES_GPGPU_COMMON_PROC_OPT
#define OGLES_GPGPU_COMMON_PROC_OPT

#include "../common_includes.h"

#include "base/multipassproc.h"
#include "multipass/gauss_opt_pass.h"

namespace ogles_gpgpu {
class GaussOptProc : public MultiPassProc {
public:
    GaussOptProc(float blurRadius = 5.0, bool doNorm=false, float normConst=0.005f) {
        GaussOptProcPass *gaussPass1 = new GaussOptProcPass(1, blurRadius, doNorm);
        GaussOptProcPass *gaussPass2 = new GaussOptProcPass(2, blurRadius, doNorm, normConst);

        procPasses.push_back(gaussPass1);
        procPasses.push_back(gaussPass2);
    }

    /**
     * Return the processors name.
     */
    virtual const char *getProcName() {
        return "GaussOptProc";
    }
};
}

#endif
