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
	
class SLICO : public SuperpixelAlgorithm
{
private:
	cv::Mat m_image;

	cv::Mat m_image_scaled;
	cv::Mat m_image_lab;

	std::vector<cv::Point> centers; // superpixel centers

	int m_region_size;
	int m_ruler;
	int m_min_element_size;
	int m_num_iterations;

	cv::Mat m_result;
	cv::Mat m_mask;
	cv::Mat m_labels;
	int m_number_of_superpixels;
public:
	SLICO(cv::Mat img, int region_size, int ruler, int min_element_size, int num_iterations);

	// Inherited via SuperpixelAlgorithm
	virtual void calculateSuperpixels() override;

	// getters
	cv::Mat getResult() const override;
	cv::Mat getMask() const override;
	cv::Mat getLabels() const override;
	int getNumber_of_superpixels() const override;


	cv::Mat colorSuperpixels() const
	{
		return cv::Mat();
	}

};
}