#include "SEEDS.h"

Superpixels::SEEDS::SEEDS(
	cv::Mat img,
	int num_iterations,
	int prior,
	bool double_step,
	int num_superpixels,
	int num_levels,
	int num_histogram_bins)
	: m_image(img)
	, m_num_iterations(num_iterations)
	, m_prior(prior)
	, m_double_step(double_step)
	, m_num_superpixels(num_superpixels)
	, m_num_levels(num_levels)
	, m_num_histogram_bins(num_histogram_bins)
{
}

void Superpixels::SEEDS::calculateSuperpixels()
{
	cv::Ptr<cv::ximgproc::SuperpixelSEEDS> seeds;
	int width, height;

	cv::Mat frame;

	m_image.copyTo(frame);

	width = frame.size().width;
	height = frame.size().height;
	seeds = cv::ximgproc::createSuperpixelSEEDS(width, height, frame.channels(), m_num_superpixels,
		m_num_levels, m_prior, m_num_histogram_bins, m_double_step);

	cv::Mat converted;
	cv::cvtColor(frame, converted, cv::COLOR_BGR2HSV);

	seeds->iterate(converted, m_num_iterations);
	m_result = frame;

	seeds->getLabels(m_labels);

	seeds->getLabelContourMask(m_mask, false);
	m_result.setTo(cv::Scalar(0, 0, 255), m_mask);

	const int num_label_bits = 2;
	m_labels &= (1 << num_label_bits) - 1;
	m_labels *= 1 << (16 - num_label_bits);
}

cv::Mat Superpixels::SEEDS::getResult() const
{
	return m_result;
}

cv::Mat Superpixels::SEEDS::getMask() const
{
	return m_mask;
}

cv::Mat Superpixels::SEEDS::getLabels() const
{
	return m_labels;
}

int Superpixels::SEEDS::getNumber_of_superpixels() const
{
	return m_num_superpixels;
}

