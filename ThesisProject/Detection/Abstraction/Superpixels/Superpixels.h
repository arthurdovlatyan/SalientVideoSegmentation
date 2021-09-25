#pragma once 

#include <opencv2/opencv.hpp>

namespace Superpixels
{

class SuperpixelAlgorithm
{
public:
	virtual void calculateSuperpixels() = 0;

	virtual cv::Mat getResult() const = 0 ;
	virtual cv::Mat getMask() const = 0;
	virtual cv::Mat getLabels() const = 0;
	virtual int getNumber_of_superpixels() const = 0;

	//virtual ~SuperpixelAlgorithm() = 0;
};
}