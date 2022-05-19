#pragma once

#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cmath>
using namespace std;

static cv::RNG MBS_RNG;

class MBS
{

public:
	MBS(const cv::Mat& src);
	cv::Mat getSaliencyMap();
	void computeSaliency(bool use_geodesic = false);
	cv::Mat getMBSMap() const { return mMBSMap; }
private:
	cv::Mat mSaliencyMap;
	cv::Mat mMBSMap;
	int mAttMapCount;
	cv::Mat mBorderPriorMap;
	cv::Mat mSrc;
	std::vector<cv::Mat> mFeatureMaps;
	void whitenFeatMap(float reg);
	void computeBorderPriorMap(float reg, float marginRatio);
};

void rasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub);
void invRasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub);
float getThreshForGeo(const cv::Mat& src);
float getThreshForGeo(const cv::Mat& src);
void invRasterScanGeo(const cv::Mat& featMap, cv::Mat& map, float thresh);

cv::Mat computeCWS(const cv::Mat src, float reg, float marginRatio);
cv::Mat fastMBS(const std::vector<cv::Mat> featureMaps);
cv::Mat fastGeodesic(const std::vector<cv::Mat> featureMaps);

int findFrameMargin(const cv::Mat& img, bool reverse);
bool removeFrame(const cv::Mat& inImg, cv::Mat& outImg, cv::Rect& roi);
cv::Mat doWork(
	const cv::Mat& src,
	bool use_lab,
	bool remove_border,
	bool use_geodesic
);