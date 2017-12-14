#pragma once
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

int arrayXihua[256] = { 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0,
1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0 };

cv::Mat VThin(cv::Mat image, int array[256])
{
	int h = image.rows;
	int w = image.cols;
	int NEXT = 1;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (NEXT == 0)
			{
				NEXT = 1;
			}
			else
			{
				int M;
				if (j > 0 && j < w - 1)
				{
					M = image.at<uchar>(i, j - 1) + image.at<uchar>(i, j) + image.at<uchar>(i, j + 1);
				}
				else
				{
					M = 1;
				}
				if (image.at<uchar>(i, j) == 0 && M != 0)
				{
					int sum;
					int a[9] = { 0 };
					for (int k = 0; k < 3; k++)
					{
						for (int l = 0; l < 3; l++)
						{
							if (i - 1 + k >(-1) && i - 1 + k < h && j - 1 + l >(-1) && j - 1 + l < w && image.at<uchar>(i - 1 + k, j - 1 + l) == 255)
							{
								a[k * 3 + l] = 1;
							}
						}
					}
					sum = a[0] * 1 + a[1] * 2 + a[2] * 4 + a[3] * 8 + a[5] * 16 + a[6] * 32 + a[7] * 64 + a[8] * 128;
					image.at<uchar>(i, j) = array[sum] * 255;
					if (array[sum] == 1)
					{
						NEXT = 0;
					}
				}
			}
		}
	}
	return image;
}

cv::Mat HThin(cv::Mat image, int array[256])
{
	int h = image.rows;
	int w = image.cols;
	int NEXT = 1;
	//for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++)
	{
		//for (int j = 0; j < w; j++)
		for (int i = 0; i < h; i++)
		{
			if (NEXT == 0)
			{
				NEXT = 1;
			}
			else
			{
				int M;
				if (i > 0 && i < h - 1)
				{
					M = image.at<uchar>(i - 1, j) + image.at<uchar>(i, j) + image.at<uchar>(i + 1, j);
				}
				else
				{
					M = 1;
				}
				if (image.at<uchar>(i, j) == 0 && M != 0)
				{
					int sum;
					int a[9] = { 0 };
					for (int k = 0; k < 3; k++)
					{
						for (int l = 0; l < 3; l++)
						{
							if (i - 1 + k >(-1) && i - 1 + k < h && j - 1 + l >(-1) && j - 1 + l < w && image.at<uchar>(i - 1 + k, j - 1 + l) == 255)
							{
								a[k * 3 + l] = 1;
							}
						}
					}
					sum = a[0] * 1 + a[1] * 2 + a[2] * 4 + a[3] * 8 + a[5] * 16 + a[6] * 32 + a[7] * 64 + a[8] * 128;
					image.at<uchar>(i, j) = array[sum] * 255;
					if (array[sum] == 1)
					{
						NEXT = 0;
					}
				}
			}
		}
	}
	return image;
}

/*
cv::Mat Xihua: 图像细化函数， 返回细化后的二值图，为Mat类型
cv::Mat image： 输入灰度图
int array[256]： 表
int num = 10： 迭代次数
*/
cv::Mat Xihua(cv::Mat image, int array[256] = arrayXihua, int num = 10)
{
	cv::Mat iXihua = image.clone();
	for (int i = 0; i < num; i++)
	{
		VThin(iXihua, array);
		HThin(iXihua, array);
	}
	return iXihua;
}

void exCon(cv::Mat img, std::vector<std::vector<cv::Point>> &con)
{
	cv::Mat src = img.clone();
	cv::threshold(src, src, 200, 255, CV_THRESH_OTSU);
	cv::bitwise_not(src, src);
	std::vector<std::vector<cv::Point>> temp;
	std::vector<cv::Point> at;
	cv::findContours(src, temp, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::drawContours(img, temp, -1, cv::Scalar(150));
	for (int i = 0; i < temp.size(); i++)
	{
		double aa = cv::contourArea(temp[i]);
		if (aa > 200)
		{
			cv::convexHull(temp[i], at);
			con.push_back(at);
		}
	}
}

