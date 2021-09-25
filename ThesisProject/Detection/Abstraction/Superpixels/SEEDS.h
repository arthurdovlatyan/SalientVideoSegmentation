#pragma once

#include "Superpixels.h"

#include <opencv2/opencv.hpp>
#include <vector>


#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>

#include <opencv2/ximgproc.hpp>

#include <ctype.h>
#include <stdio.h>
#include <iostream>

namespace Superpixels
{

class SEEDS : public SuperpixelAlgorithm
{
private:
	cv::Mat m_image;

	int m_num_iterations;
	int m_prior;
	bool m_double_step;
	int m_num_superpixels;
	int m_num_levels;
	int m_num_histogram_bins;

	cv::Mat m_result;
	cv::Mat m_mask;
	cv::Mat m_labels;

public:
	SEEDS(cv::Mat img, 
	int num_iterations,
	int prior,
	bool double_step,
	int num_superpixels,
	int num_levels,
	int num_histogram_bins);

	// Inherited via SuperpixelAlgorithm
	virtual void calculateSuperpixels() override;

	virtual cv::Mat getResult() const override;

	virtual cv::Mat getMask() const override;

	virtual cv::Mat getLabels() const override;

	virtual int getNumber_of_superpixels() const override;

};

}