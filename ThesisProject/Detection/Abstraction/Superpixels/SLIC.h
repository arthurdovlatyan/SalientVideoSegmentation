#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>


#include "Superpixels.h"

#define DEFAULT_M 20
#define USE_DEFAULT_S -1


namespace Superpixels
{

class SLIC : public SuperpixelAlgorithm {

public:
    SLIC(cv::Mat& img, float m = DEFAULT_M, float S = USE_DEFAULT_S); // calculates the superpixel boundaries on construction

    cv::Mat viewSuperpixels() const; // returns image displaying superpixel boundaries
    cv::Mat colorSuperpixels() const; // recolors image with average color in each cluster
    // map<int, Point>  getCentersMap(); // returns the labels and their cluster centers
    std::vector<cv::Point> getCenters(); // centers indexed by label
    cv::Mat getLabels() const override; // per pixel label

    cv::Mat getResult() const
    {
        cv::Mat res = viewSuperpixels();
        return res;
    }
    cv::Mat getMask() const
    {
        return cv::Mat();
    }
    int getNumber_of_superpixels() const
    {
        return centers.size();
    }


    void calculateSuperpixels() override;
protected:
    cv::Mat img; // original image
    cv::Mat img_f; // scaled to [0,1]
    cv::Mat img_lab; // converted to LAB colorspace

    // used to store the calculated results
    cv::Mat show;
    cv::Mat labels;

    float m; // compactness parameter
    float S; // window size

    int nx, ny; // cols and rows
    float dx, dy; // steps

    std::vector<cv::Point> centers; // superpixel centers

    
    float dist(cv::Point p1, cv::Point p2); // 5-D distance between pixels in LAB space

    const static cv::Mat sobel;
};
}