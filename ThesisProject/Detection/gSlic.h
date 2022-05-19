#pragma once

#include "Image.h"
#include "opencl.h"


void slicSuperPixels(GPUAcceleration::OpenCL& opencl,
    const Size& imgSize, int stride,
    int superPixelSpacing, int nIters, float relWeight,
    GPUAcceleration::Memory& img, GPUAcceleration::Memory& clusterCenters, GPUAcceleration::Memory& clusterAssig);