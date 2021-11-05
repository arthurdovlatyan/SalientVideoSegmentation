#include "SLICO.h"

Superpixels::SLICO::SLICO(cv::Mat img, 
	int region_size, 
	int ruler, 
	int min_element_size, 
	int num_iterations)
	: m_image(img)
	, m_region_size(region_size)
	, m_ruler(ruler)
	, m_min_element_size(min_element_size)
	, m_num_iterations(num_iterations)
{
}

void Superpixels::SLICO::calculateSuperpixels()
{
	cv::Mat frame = m_image;

	m_result = frame;
	cv::Mat converted;
	cvtColor(frame, converted, cv::COLOR_BGR2HSV);

	cv::Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(converted, cv::ximgproc::SLICO, m_region_size, float(m_ruler));

	slic->iterate(m_num_iterations);

	if (m_min_element_size > 0)
	{
		slic->enforceLabelConnectivity(m_min_element_size);
	}

	m_number_of_superpixels = slic->getNumberOfSuperpixels();

	// get the contours for displaying
	slic->getLabelContourMask(m_mask, true);
	m_result.setTo(cv::Scalar(0, 0, 255), m_mask);

	// get labels
	slic->getLabels(m_labels);
	const int num_label_bits = 2;
	m_labels &= (1 << num_label_bits) - 1;
	m_labels *= 1 << (16 - num_label_bits);
}


cv::Mat Superpixels::SLICO::getResult() const
{
	return m_result;
}

cv::Mat Superpixels::SLICO::getMask() const
{
	return m_mask;
}

cv::Mat Superpixels::SLICO::getLabels() const
{
	return m_labels;
}

int Superpixels::SLICO::getNumber_of_superpixels() const
{
	return m_number_of_superpixels;
}