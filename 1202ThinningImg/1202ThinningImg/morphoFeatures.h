#pragma once
#if ! defined MORPHOF
#define MORPHOF

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class MorphoFeatures
{
private:
	//������ֵͼ�������
	int threshold;
	//�ṹԪ
	Mat cross;
	Mat diamond;
	Mat square;
	Mat x;
	//��ͼ���ֵ��
	void applyThreshold(Mat &result)
	{
		if (threshold>0)
			cv::threshold(result, result, threshold, 255, THRESH_BINARY_INV);
	}


public:
	//���캯��
	MorphoFeatures() :threshold(-1),
		cross(5, 5, CV_8U, Scalar(0)),
		diamond(5, 5, CV_8U, Scalar(255)),
		square(5, 5, CV_8U, Scalar(255)),
		x(5, 5, CV_8U, Scalar(0))
	{
		//����ʮ���ͽṹԪ
		for (int i = 0; i < 5; i++)
		{
			cross.at<uchar>(2, i) = 255;
			cross.at<uchar>(i, 2) = 255;
		}

		//�������νṹԪ���ֶ�����ֻ��Ҫ�Ѳ������εĲ��ֱ�׼���
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

		//����X��
		for (int i = 0; i < 5; i++)
		{
			//���Խ���
			x.at<uchar>(i, i) = 255;
			//���Խ���
			x.at<uchar>(4 - i, i) = 255;
		}

	}

	//�������޺���
	void setThreshold(int t)
	{
		threshold = t;
	}

	//��ȡ��ǰ����
	int getThreshold() const
	{
		return threshold;
	}

	//���ֱ�ߺ���
	Mat getEdges(const Mat &image)
	{
		Mat result;
		//��ȡͼ����ݶ�
		morphologyEx(image, result, cv::MORPH_GRADIENT, Mat());
		//�����ֵ��
		applyThreshold(result);
		return result;
	}

	//���ǵ㺯��
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

	//�ڽǵ㴦��Բ
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