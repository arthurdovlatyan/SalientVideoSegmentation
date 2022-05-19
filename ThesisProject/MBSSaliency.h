#pragma once
#include "MBS.h"

#include <ctime>

using namespace std;
void Reconstruct(cv::Mat src, cv::Mat mask, cv::Mat& dst);

cv::Mat morpySmooth(cv::Mat I, int radius);

cv::Mat enhanceConstrast(cv::Mat I, int b = 10);
