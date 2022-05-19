#include "MBSSaliency.h"
using namespace std;
void Reconstruct(cv::Mat src, cv::Mat mask, cv::Mat& dst)
{
	using namespace cv;
	/*function:morpy-Reconstruct
	src is the source img£¬mask is label img£¬dst is the output*/
	cv::Mat se = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
	cv::Mat tmp1(src.size(), src.type()), tmp2(src.size(), src.type());
	cv::min(src, mask, dst);
	do
	{
		dst.copyTo(tmp1);
		dilate(dst, mask, se);
		cv::min(src, mask, dst);
		tmp2 = abs(tmp1 - dst);
	} while (sum(tmp2).val[0] != 0);
}
cv::Mat morpySmooth(cv::Mat I, int radius)
{
	using namespace cv;
	cv::Mat openmat, recon1, dilatemat, recon2, res;
	I = I.mul(255);
	I.convertTo(I, CV_8UC1);
	morphologyEx(I, openmat, MORPH_OPEN, cv::Mat(radius, radius, I.type()), Point(-1, -1), 1);
	Reconstruct(I, openmat, recon1);
	dilate(recon1, dilatemat, cv::Mat(radius, radius, I.type()), Point(-1, -1), 1);

	recon1 = 255 - recon1;
	dilatemat = 255 - dilatemat;

	Reconstruct(recon1, dilatemat, recon2);

	res = 255 - recon2;
	return res;
}
cv::Mat enhanceConstrast(cv::Mat I, int b)
{
	using namespace cv;
	I.convertTo(I, CV_32FC1);
	int total = I.rows * I.cols, num1(0), num2(0);
	double max, min, t, sum1(0), sum2(0), v1, v2;
	Point p1, p2;
	minMaxLoc(I, &min, &max, &p1, &p2);
	t = max * 0.5;

	for (int i = 0; i < I.rows; i++)
	{
		float* indata = I.ptr<float>(i);
		for (int j = 0; j < I.cols; j++)
		{
			int temp = (*indata);
			if (temp >= t)
			{
				sum1 += temp;
				num1++;
			}

			else
			{
				sum2 += temp;
				num2++;
			}

			indata++;
		}
	}
	v1 = sum1 / num1;
	v2 = sum2 / num2;
	//cout<<t<<" "<<v1<<" "<<v2<<endl;
	for (int i = 0; i < I.rows; i++)
	{
		float* indata = I.ptr<float>(i);
		for (int j = 0; j < I.cols; j++)
		{
			*indata = 1.0 / (exp(((*indata) - 0.5 * (v1 + v2)) * (-b)) + 1) * 255;
			indata++;
		}
	}
	//cout<<I<<endl;
	return I;
}
