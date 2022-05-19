#pragma once

#include ""

Size superPixelGridSize(const Size& imageSize, int superPixelSpacing);

void slicSuperPixels(OpenCL& opencl,
    const Size& imgSize, int stride,
    int superPixelSpacing, int nIters, float relWeight,
    Memory& img, Memory& clusterCenters, Memory& clusterAssig);