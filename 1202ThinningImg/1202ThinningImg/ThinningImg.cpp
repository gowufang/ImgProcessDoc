//基于索引表的图像细化算法

#include <highgui.h>   
#include <windows.h>   
#include "contours.h"
#include "highgui.h"
#include "cv.h"//包含cvCreateStructuringElementEx();颜色空间转换函数cvCvtColor()需要
#include "cxcore.h"
#include"opencv2/imgproc/imgproc.hpp"  
#include <stdlib.h>
#include <string.h>
#include "Thinning.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ThresholdWithOtsu.h"
#include "ThinningAlgorithm.h"
#include "morphoFeatures.h"
#include "ThinningSister.h"
using namespace cv;






//去除二值图像边缘的突出部
//uthreshold、vthreshold分别表示突出部的宽度阈值和高度阈值
//type代表突出部的颜色，0表示黑色，1代表白色 
void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type)
{
	int threshold;
	src.copyTo(dst);
	int height = dst.rows;
	int width = dst.cols;
	int k;  //用于循环计数传递到外部
	for (int i = 0; i < height - 1; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width - 1; j++)
		{
			if (type == 0)
			{
				//行消除
				if (p[j] == 255 && p[j + 1] == 0)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 255;
						}
					}
				}
				//列消除
				if (p[j] == 255 && p[j + width] == 0)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold*width; k += width)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 255;
						}
					}
				}
			}
			else  //type = 1
			{
				//行消除
				if (p[j] == 0 && p[j + 1] == 255)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 0) break;
						}
						if (p[k] == 0)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 0;
						}
					}
				}
				//列消除
				if (p[j] == 0 && p[j + width] == 255)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold*width; k += width)
						{
							if (p[k] == 0) break;
						}
						if (p[k] == 0)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 0;
						}
					}
				}
			}
		}
	}
}

//图片边缘光滑处理
//size表示取均值的窗口大小，threshold表示对均值图像进行二值化的阈值
void imageblur(Mat& src, Mat& dst, Size size, int threshold)
{
	int height = src.rows;
	int width = src.cols;
	//blur(src, dst, size);
	for (int i = 0; i < height; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (p[j] < threshold)
				p[j] = 0;
			else p[j] = 255;
		}
	}
	imshow("Blur", dst);
}



/**
 * @brief 对输入图像进行细化
  * @param src为输入图像,用cvThreshold函数处理过的8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
   * @param maxIterations限制迭代次数，如果不进行限制，默认为-1，代表不限制迭代次数，直到获得最终结果
    * @return 为对src细化后的输出图像,格式与src格式相同，元素中只有0与1,1代表有元素，0代表为空白
	 */
cv::Mat thinImage(const cv::Mat & src, const int maxIterations = -1)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达
			break;
		std::vector<uchar *> mFlag; //用于标记需要删除的点
									//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}

		//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}
	}
	return dst;
}




int main(int argc, char* argv[])
{
	int flag = 0;
	//加载原图像
	IplImage* src = cvLoadImage("06100018/00000113(226,653,352,791).jpg", 0);
	


	//---------------------------预处理二值化------------------------------------
	/*对原图像进行二值化  阈值
	cvThreshold(src, src, 100, 255, CV_THRESH_BINARY);

	图像二值化就是将图像上的像素点的灰度值设置为0或255，
	也就是将整个图像呈现出明显的黑白效果
	CV_THRESH_BINARY  意思为： if src(x,y) > 100
	src(x,y)=255
	else
	src(x,y)=-0
	原图像由0到255---->0或255
	*/

	//三种滤波操作
	//boxFilter(img, dst, -1, Size(5, 5));
	//blur(img, dst, Size(7, 7));
	//GaussianBlur(img, dst, Size(5, 5), 0, 0);

	
	unsigned int thresValue = Otsu((unsigned char *)src->imageData, src->width, src->height, src->widthStep);

	Thresholding((unsigned char *)src->imageData, src->width, src->height, src->widthStep,
		thresValue);

	//Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	//erode(img, dst, element);

	Mat img = cvarrToMat(src);
	Mat dst = img;

	/*test****************************************************/
	imshow("Mat Img", dst);//得到的是二值化的图
    /*test-end************************************************/
	//delete_jut(img, dst, 1, 1, 0);//去除二值图像边缘的突出部  
	
	//Mat dst2;
	//dst2 = thinImage(img);
	////平滑处理
	//imageblur(img, dst, Size(10, 10), thresValue);
	//imshow("【效果图】膨胀操作", dst2);
	//二值化的图像
	cvNamedWindow("Threshold", 0);
	cvShowImage("Threshold", src);

	//ThingSister
	//Mat dst_thin = Xihua(img, arrayXihua);
	Mat dst_thin=Xihua(dst, arrayXihua);
	imshow("thinning_sister", dst_thin);






	//FillInternalContours(src, 20);//轮廓填充
	//cvNamedWindow("Contours");
	//cvShowImage("Contours", src);

	//tuchubuixaochu








	//动态创建一个长度为sizeof(char) * src->width * src->height的字符数组
	//并且初始化把数组元素都设置为0
	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	

	//case 3 yongdao
	Mat edges;
	MorphoFeatures morpho;
	Mat corners;


	



	scanf("%d", &flag);
	//----------------------------细化-----------------------------------------

	//经过预处理后得到待细化的图像是0、1二值图像。

	//细化一次，和细化多次，由于索引表不变，每次细化效果都一样，想采用多次细化无法消除毛刺
	
	
	switch (flag)
	{
	case 0:
		//-----------------------------预处理0/1化-------------------------------------
		/*
		将imagedata指向的与原图像大小一下的数组空间进行0或1赋值
		二值图有原来的0或255----->------0或1
		*/

		

		int x, y;
		for (y = 0; y<src->height; y++)
		{
			/*
			widthStep表示存储一行像素需要的字节数
			偏移到第y行
			*/
			unsigned char* ptr = (unsigned char*)(src->imageData + y*src->widthStep);

			for (x = 0; x<src->width; x++)
			{
				/*
				判断第y行第x列的元素值是否大于0
				大于0时将数组对应的位置元素设置为1
				不大于0则设置为0

				imagedata数组里面不是0就时1
				*/
				imagedata[y*src->width + x] = ptr[x] > 0 ? 1 : 0;
				//经过预处理后得到待细化的图像是0、1二值图像。
			}
		}

		ThiningDIBSkeleton(imagedata, src->width, src->height);
		//--------------------对细化后的0/1图二值化(0/255)-----------------------------
		/*
		将ptr指向的与原图像大小一下的数组空间进行0或255赋值
		*/
		for (y = 0; y<src->height; y++)
		{
			/*
			widthStep表示存储一行像素需要的字节数
			偏移到第y行
			*/
			unsigned char* ptr = (unsigned char*)(src->imageData + y*src->widthStep);

			for (x = 0; x<src->width; x++)//width:从左到右
			{
				/*
				判断第y行第x列的元素值是否大于0
				大于0时将数组对应的位置元素设置为1
				不大于0则设置为0
canny边缘检测
				ptr指向的空间的值不是0就是255
				也就是细化之后再次变为二值图
				*/
				ptr[x] = imagedata[y*src->width + x]>0 ? 255 : 0;
			}

		}

		//-------------------------------------------------------------------------
		//ZhangThinning(src->width, src->height, (unsigned char *)src->imageData);
		//blur(img, dst, Size(7, 7));
		//delete_jut(img, dst, 3.5, 3.5, 0);//去除二值图像边缘的突出部  
		//imageblur(img, dst, Size(10, 10), thresValue);
		

		break;
	case 1:
		ZhangThinning(src->width, src->height, (unsigned char *)src->imageData);
		
		break;
		
	case 2:
		//ThinnerHilditch((unsigned char *)src->imageData, src->width, src->height);
		//ThinnerPavlidis((unsigned char *)src->imageData, src->width, src->height);
		//ThinnerRosenfeld((unsigned char *)src->imageData, src->width, src->height);
		//ThiningDIBSkeleton((unsigned char *)src->imageData, src->width, src->height);
		break;
	case 3:
		//获取边沿,轮廓
		
		edges = morpho.getEdges(img);
		imshow("边沿", edges);
		//获取角点
		morpho.setThreshold(-1);
		
		corners = morpho.getCorners(img);
		morphologyEx(corners, corners, MORPH_TOPHAT, Mat());
		threshold(corners, corners, 40, 255, THRESH_BINARY_INV);
		//imshow("角点",corners);

		//展示图片上的角点
		morpho.drawOnImage(corners, img);
		imshow("图片上的角点", img);
		break;
	case 4:
		dst_thin= Xihua(img, arrayXihua);
		imshow("thinning_sister", dst_thin);
		break;
	default:
		break;
	}
	


	cvNamedWindow("Skeleton", 0);
	cvShowImage("Skeleton", src);
	cvWaitKey(0);

	if (flag = 0) {
		delete[]imagedata;
	}

	return 0;
}