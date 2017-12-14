#pragma once
#if ! defined MORPHOF
#define MORPHOF

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class MorphoFeatures
{
private:
	//产生二值图像的门限
	int threshold;
	//结构元
	Mat cross;
	Mat diamond;
	Mat square;
	Mat x;
	//对图像二值化
	void applyThreshold(Mat &result)
	{
		if (threshold>0)
			cv::threshold(result, result, threshold, 255, THRESH_BINARY_INV);
	}


public:
	//构造函数
	MorphoFeatures() :threshold(-1),
		cross(5, 5, CV_8U, Scalar(0)),
		diamond(5, 5, CV_8U, Scalar(255)),
		square(5, 5, CV_8U, Scalar(255)),
		x(5, 5, CV_8U, Scalar(0))
	{
		//创建十字型结构元
		for (int i = 0; i < 5; i++)
		{
			cross.at<uchar>(2, i) = 255;
			cross.at<uchar>(i, 2) = 255;
		}

		//创建菱形结构元：手动画，只需要把不是菱形的部分变白即可
		diamond.at<uchar>(0, 0) = 0;
		diamond.at<uchar>(0, 1) = 0;
		diamond.at<uchar>(1, 0) = 0;
		diamond.at<uchar>(4, 4) = 0;
		diamond.at<uchar>(3, 4) = 0;
		diamond.at<uchar>(4, 3) = 0;
		diamond.at<uchar>(4, 0) = 0;
		diamond.at<uchar>(4, 1) = 0;
		diamond.at<uchar>(3, 0) = 0;
		diamond.at<uchar>(0, 4) = 0;
		diamond.at<uchar>(0, 3) = 0;
		diamond.at<uchar>(1, 4) = 0;

		//创建X形
		for (int i = 0; i < 5; i++)
		{
			//主对角线
			x.at<uchar>(i, i) = 255;
			//副对角线
			x.at<uchar>(4 - i, i) = 255;
		}

	}

	//设置门限函数
	void setThreshold(int t)
	{
		threshold = t;
	}

	//获取当前门限
	int getThreshold() const
	{
		return threshold;
	}

	//检测直线函数
	Mat getEdges(const Mat &image)
	{
		Mat result;
		//获取图像的梯度
		morphologyEx(image, result, cv::MORPH_GRADIENT, Mat());
		//结果二值化
		applyThreshold(result);
		return result;
	}

	//检测角点函数
	Mat getCorners(const Mat &image)
	{
		Mat result;
		dilate(image, result, cross);
		erode(result, result, diamond);
		Mat result2;
		dilate(image, result2, x);
		erode(result2, result2, square);
		absdiff(result2, result, result);
		applyThreshold(result);
		return result;
	}

	//在角点处画圆
	void drawOnImage(const Mat &binary, Mat &image)
	{
		Mat_<uchar>::const_iterator it = binary.begin<uchar>();
		Mat_<uchar>::const_iterator itend = binary.end<uchar>();
		for (int i = 0; it != itend; ++it, ++i)
		{
			if (!*it)
				circle(image, Point(i%image.step, i / image.step), 5, Scalar(255, 0, 0));
		}
	}

};



#endif