//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// Author: Markus Konrad <post@mkonrad.net>, Winter 2014/2015
// http://www.mkonrad.net
//
// See LICENSE file in project repository root for the license.
//

// Modifications: Copyright (c) 2015-2017, David Hirvonen (this file)

/**
 * GPGPU gaussian smoothing processor (two-pass).
 */
#ifndef OGLES_GPGPU_COMMON_PROC_GAUSS
#define OGLES_GPGPU_COMMON_PROC_GAUSS

#include "../common_includes.h"

#include "base/multipassproc.h"
#include "multipass/gauss_pass.h"

namespace ogles_gpgpu {
class GaussProc : public MultiPassProc {
public:
    GaussProc(GaussProcPass::KernelSize kernel = GaussProcPass::k5Tap, bool doR = false) {
        GaussProcPass* gaussPass1 = new GaussProcPass(1, kernel, doR);
        GaussProcPass* gaussPass2 = new GaussProcPass(2, kernel, doR);

        procPasses.push_back(gaussPass1);
        procPasses.push_back(gaussPass2);
    }

    /**
     * Return the processors name.
     */
    virtual const char* getProcName() {
        return "GaussProc";
    }
};
}

#endif
