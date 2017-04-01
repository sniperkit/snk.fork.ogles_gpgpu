//
// ogles_gpgpu project - GPGPU for mobile devices and embedded systems using OpenGL ES 2.0
//
// See LICENSE file in project repository root for the license.
//

// Copyright (c) 2016-2017, David Hirvonen (this file)

#ifndef OGLES_GPGPU_COMMON_PROC_BOX_OPT
#define OGLES_GPGPU_COMMON_PROC_BOX_OPT

#include "../common_includes.h"

#include "base/multipassproc.h"
#include "multipass/box_opt_pass.h"

namespace ogles_gpgpu {
class BoxOptProc : public MultiPassProc {
public:
    BoxOptProc(float blurRadius = 5.0) {
        BoxOptProcPass *boxPass1 = new BoxOptProcPass(1, blurRadius);
        BoxOptProcPass *boxPass2 = new BoxOptProcPass(2, blurRadius);

        procPasses.push_back(boxPass1);
        procPasses.push_back(boxPass2);
    }

    /**
     * Return the processors name.
     */
    virtual const char *getProcName() {
        return "BoxOptProc";
    }
};
}

#endif
