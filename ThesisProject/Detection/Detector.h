#pragma once

#include <memory>

#include <opencv2/opencv.hpp>

#include "Abstraction/Superpixels/Superpixels.h"
#include "Abstraction/Superpixels/SLICO.h"
#include "Abstraction/Superpixels/SEEDS.h"
#include "Abstraction/Superpixels/SLIC.h"


#include "opencl.h"

namespace SalientDetector
{

struct SuperpixelAlgorithmResult
{
    cv::Mat m_result;
    cv::Mat m_mask;
    cv::Mat m_labels;
};

class SalientDetector
{
private:
    cv::Mat m_orig_image;
    std::unique_ptr<Superpixels::SuperpixelAlgorithm> m_superpixelAlgorithm;
    SuperpixelAlgorithmResult m_superpixelAbstaction;
    cv::Mat m_imageColored;

    static const char* kernelSources;

    bool SLIC = false;

    cv::Mat m_uniqueness;
    cv::Mat m_distribution;
    cv::Mat m_saliency;

protected:
    void setSuperpixelAbstraction();

public:
    SalientDetector() = default;
    SalientDetector(cv::Mat img);

    void setMainImage(cv::Mat& img);

    void applySLICO(int region_size, int ruler, int min_element_size, int num_iterations);

    void applySEEDS(int num_iterations,
        int prior,
        bool double_step,
        int num_superpixels,
        int num_levels,
        int num_histogram_bins);

    void applySLIC();

    SuperpixelAlgorithmResult getResult();

    cv::Mat colorSupepixels();

    //cv::Mat applyUniqueness(bool useGPU = true);

    //cv::Mat applyDistribution(bool useGPU = true);

    cv::Mat applySaliency(bool useGPU = false);

private:
    void saliencyFiltersSP(GPUAcceleration::OpenCL& opencl,
        const cv::Size& gridSize,
        GPUAcceleration::Memory& clusterCenters,
        GPUAcceleration::Memory& saliencySP,
        float stdDevUniqueness = 0.25,
        float stdDevDistribution = 2.0,
        float k = 6.0);

    void propagateSaliency(GPUAcceleration::OpenCL& opencl,
        const cv::Size& imgSize,
        int imgStride,
        const cv::Size& gridSize,
        GPUAcceleration::Memory& img,
        GPUAcceleration::Memory& clusterAssig,
        GPUAcceleration::Memory& saliencySP,
        GPUAcceleration::Memory& saliency,
        float alpha,
        float beta);

    static void elementUniqueness(GPUAcceleration::OpenCL& opencl,
            const cv::Size& gridSize,
            GPUAcceleration::Memory& clusterCenters, 
            GPUAcceleration::Memory& uniqueness,
            float stdDevUniqueness);

    static void elementDistribution(GPUAcceleration::OpenCL& opencl,
            const cv::Size& gridSize,
            GPUAcceleration::Memory& clusterCenters,
            GPUAcceleration::Memory& distribution,
            float stdDevDistribution);

    static void elementSaliency(GPUAcceleration::OpenCL& opencl,
            const cv::Size& gridSize,
            GPUAcceleration::Memory& uniqueness, 
            GPUAcceleration::Memory& distribution,
            GPUAcceleration::Memory& saliency,
            float k);

    static void executeKernel(const GPUAcceleration::OpenCL& opencl, const GPUAcceleration::Kernel& kernel, size_t domainSize)
    {
        std::vector<size_t> tmp(1, domainSize);
        executeKernel(opencl, kernel, tmp);
    }

    static void executeKernel(const GPUAcceleration::OpenCL& opencl, const GPUAcceleration::Kernel& kernel, const std::vector<size_t>& domainSize)
    {
        int err = clEnqueueNDRangeKernel(opencl.getCommandsQueue(), kernel.getKernel(), domainSize.size(), NULL, &domainSize[0], NULL, 0, NULL, NULL);
        if (err) /*LOG_FATAL("Failed to execute kernel: " << openCLErrorString(err))*/;

        clFinish(opencl.getCommandsQueue());
    }
};
}