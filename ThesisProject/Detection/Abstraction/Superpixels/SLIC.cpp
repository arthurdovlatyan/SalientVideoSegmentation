#include "SLIC.h"

#include "opencv2/ximgproc.hpp"

const cv::Mat Superpixels::SLIC::sobel = (cv::Mat_<float>(3, 3) << -1 / 16., -2 / 16., -1 / 16., 0, 0, 0, 1 / 16., 2 / 16., 1 / 16.);

Superpixels::SLIC::SLIC(cv::Mat& img, float m, float S) {
    this->img = img.clone();
    this->m = m;
    if (S == USE_DEFAULT_S) {
        this->nx = 100; // cols
        this->ny = 100; // rows
        this->dx = img.cols / float(nx); //steps
        this->dy = img.rows / float(ny);
        this->S = (dx + dy + 1) / 2; // default window size
    }
    else
        this->S = S;
}

cv::Mat Superpixels::SLIC::viewSuperpixels() const 
{

    // Draw boundaries on original image
    std::vector<cv::Mat> rgb(3);
    split(this->img_f, rgb);
    for (int i = 0; i < 3; i++) {
        rgb[i] = rgb[i].mul(this->show);
    }

    cv::Mat output = this->img_f.clone();
    merge(rgb, output);

    output = 255 * output;
    output.convertTo(output, CV_8UC3);

    return output;
}

cv::Mat Superpixels::SLIC::colorSuperpixels() const 
{

    int n = nx * ny;
    std::vector<cv::Vec3b> avg_colors(n);
    std::vector<int> num_pixels(n);

    std::vector<long> b(n), g(n), r(n);

    for (int y = 0; y < (int)labels.rows; ++y) {
        for (int x = 0; x < (int)labels.cols; ++x) {

            cv::Vec3b pix = img.at<cv::Vec3b>(y, x);
            int lbl = labels.at<int>(y, x);

            b[lbl] += (int)pix[0];
            g[lbl] += (int)pix[1];
            r[lbl] += (int)pix[2];

            ++num_pixels[lbl];
        }
    }

    for (int i = 0; i < n; ++i) {
        int num = num_pixels[i];
        avg_colors[i] = cv::Vec3b(b[i] / num, g[i] / num, r[i] / num);
    }

    cv::Mat output = this->img.clone();
    for (int y = 0; y < (int)output.rows; ++y) {
        for (int x = 0; x < (int)output.cols; ++x) {
            int lbl = labels.at<int>(y, x);
            if (num_pixels[lbl])
                output.at<cv::Vec3b>(y, x) = avg_colors[lbl];
        }
    }

    return output;
}

std::vector<cv::Point> Superpixels::SLIC::getCenters()
{
    return centers;
}

cv::Mat Superpixels::SLIC::getLabels() const
{
    return labels;
}

// map<int, Point> Superpixels::getCentersMap(){

//     map<int, Point> out;
//     for(int i = 0; i < (int) centers.size(); ++i){
//         Point p = centers[i];
//         int lbl = labels.at<int>(p);
//         out[lbl] = p;
//     }
//     return out;
// }

void Superpixels::SLIC::calculateSuperpixels() {

    // Scale img to [0,1]
    this->img.convertTo(this->img_f, CV_32F, 1 / 255.);

    // Convert to l-a-b colorspace
    cv::cvtColor(this->img_f, this->img_lab, cv::COLOR_BGR2Lab);

    int n = nx * ny;
    int w = img.cols;
    int h = img.rows;

    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            this->centers.push_back(cv::Point2f(j * dx + dx / 2, i * dy + dy / 2));
        }
    }

    // Initialize labels and distance maps
    std::vector<int> label_vec(n);
    for (int i = 0; i < n; i++)
        label_vec[i] = i * 255 * 255 / n;

    cv::Mat labels = -1 * cv::Mat::ones(this->img_lab.size(), CV_32S);
    cv::Mat dists = -1 * cv::Mat::ones(this->img_lab.size(), CV_32F);
    cv::Mat window;
    cv::Point2i p1, p2;
    cv::Vec3f p1_lab, p2_lab;

    // Iterate 10 times. In practice more than enough to converge
    for (int i = 0; i < 10; i++) {
        // For each center...
        for (int c = 0; c < n; c++)
        {
            int label = label_vec[c];
            p1 = centers[c];
            int xmin = std::max<int>(p1.x - S, 0);
            int ymin = std::max<int>(p1.y - S, 0);
            int xmax = std::min<int>(p1.x + S, w - 1);
            int ymax = std::min<int>(p1.y + S, h - 1);

            // Search in a window around the center
            window = this->img_f(cv::Range(ymin, ymax), cv::Range(xmin, xmax));

            // Reassign pixels to nearest center
            for (int i = 0; i < window.rows; i++) {
                for (int j = 0; j < window.cols; j++) {
                    p2 = cv::Point2i(xmin + j, ymin + i);
                    float d = dist(p1, p2);
                    float last_d = dists.at<float>(p2);
                    if (d < last_d || last_d == -1) {
                        dists.at<float>(p2) = d;
                        labels.at<int>(p2) = label;
                    }
                }
            }
        }
    }

    // Store the labels for each pixel
    this->labels = labels.clone();
    this->labels = n * this->labels / (255 * 255);

    // Calculate superpixel boundaries
    labels.convertTo(labels, CV_32F);

    cv::Mat gx, gy, grad;
    filter2D(labels, gx, -1, sobel);
    filter2D(labels, gy, -1, sobel.t());
    magnitude(gx, gy, grad);
    grad = (grad > 1e-4) / 255;
    cv::Mat show = 1 - grad;
    show.convertTo(show, CV_32F);

    // Store the result
    this->show = show.clone();
}

float Superpixels::SLIC::dist(cv::Point p1, cv::Point p2) {
    cv::Vec3f p1_lab = this->img_lab.at<cv::Vec3f>(p1);
    cv::Vec3f p2_lab = this->img_lab.at<cv::Vec3f>(p2);

    float dl = p1_lab[0] - p2_lab[0];
    float da = p1_lab[1] - p2_lab[1];
    float db = p1_lab[2] - p2_lab[2];

    float d_lab = sqrtf(dl * dl + da * da + db * db);

    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;

    float d_xy = sqrtf(dx * dx + dy * dy);

    return d_lab + m / S * d_xy;
}
