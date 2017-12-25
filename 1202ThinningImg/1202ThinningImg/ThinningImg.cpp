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
#include <vector>   
#include <iostream> 
using namespace std;
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


//thinImage没有效果
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


IplImage * SmoothImage(char* strFileName)
{
	///////////////////////////************去掉独立噪声点*****************/////////////////////////////
	//去孤立小点
	const int NOISE_MAX_AREA = 100;//噪声点最大值，像素
	const int CONTOUR_MAX_AREA = 800; //数字矩形一般为19*55

	const int MAXCHARCOUNT = 20;  //最多矩形的个数
	int intRealCharCount = 0;
	IplImage* src;
	IplImage* dst2;
	IplImage* tmpsrc1;
	IplImage* tmpsrc2;
	IplImage* tmpsrc3;

	if ((src = cvLoadImage(strFileName, 0)) != 0)
	{

		dst2 = cvCloneImage(src);
		tmpsrc1 = cvCloneImage(src);
		tmpsrc2 = cvCreateImage(cvGetSize(tmpsrc1), 8, 3);
		cvZero(tmpsrc2);
		tmpsrc3 = cvCreateImage(cvGetSize(tmpsrc1), 8, 3);
		cvZero(tmpsrc3);
		cvNot(tmpsrc3, tmpsrc3); //取反，使全白
								 //去噪声
		cvSmooth(tmpsrc1, tmpsrc1, CV_MEDIAN, 3, 3, 0);

		//边缘检测
		cvCanny(tmpsrc1, tmpsrc1, 50, 150, 3);



		//画矩形
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		//二值化图像
		cvThreshold(tmpsrc1, tmpsrc1, 1, 255, CV_THRESH_BINARY);

		cvFindContours(tmpsrc1, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		for (; contour; contour = contour->h_next)
		{
			CvRect r = ((CvContour*)contour)->rect;

			if (r.width*r.height <NOISE_MAX_AREA)
			{
				CvScalar s1;
				for (int i = r.y; i<r.y + r.height; i++)
				{
					for (int j = r.x; j<r.x + r.width; j++)
					{
						s1 = cvGet2D(tmpsrc3, i, j);
						cvSet2D(dst2, i, j, s1);
					}
				}
			}
			if (r.height*r.width>CONTOUR_MAX_AREA)
			{
				cvRectangle(tmpsrc2, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y + r.height), CV_RGB(255, 0, 0), 1, CV_AA, 0);
			}
		}

		cvNamedWindow("dst", 1);
		cvShowImage("dst", dst2);


		cvSaveImage(strFileName, dst2);

		cvReleaseImage(&tmpsrc1);
		cvReleaseImage(&tmpsrc2);
		cvReleaseImage(&tmpsrc3);
		return src;
	}
	///////////////////////////************去掉独立噪声点*****************/////////////////////////////
}

//CheckMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;  
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;       //记录除去的个数  
							   //记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查  
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);

	if (CheckMode == 1)
	{
		cout << "Mode: 去除小区域.";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] < 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}
	else
	{
		cout << "Mode: 去除孔洞. ";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] > 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}

	vector<Point2i> NeihborPos;  //记录邻域点位置  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********  
				vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点  
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出  

				for (int z = 0; z<GrowBuffer.size(); z++)
				{

					for (int q = 0; q<NeihborCount; q++)                                      //检查四个邻域点  
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界  
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer  
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查  
							}
						}
					}

				}
				if (GrowBuffer.size()>AreaLimit) CheckResult = 2;                 //判断结果（是否超出限定的大小），1为未超出，2为超出  
				else { CheckResult = 1;   RemoveCount++; }
				for (int z = 0; z<GrowBuffer.size(); z++)                         //更新Label记录  
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					Pointlabel.at<uchar>(CurrY, CurrX) += CheckResult;
				}
				//********结束该点处的检查**********  


			}
		}
	}

	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域  
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iData = Src.ptr<uchar>(i);
		uchar* iDstData = Dst.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 2)
			{
				iDstData[j] = CheckMode;
			}
			else if (iLabel[j] == 3)
			{
				iDstData[j] = iData[j];
			}
		}
	}

	cout << RemoveCount << " objects removed." << endl;
}
int main(int argc, char* argv[])
{
	int flag = 0;//flag默认为0
	//加载原图像
	IplImage* src = cvLoadImage("06100018/00000115(534,694,658,828).jpg", 0);
	/*char * strFileName = "06100018/00000115(534,694,658,828).jpg";

	IplImage* src = SmoothImage(strFileName);*/




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

	//threshold(img, img, 100, 255, CV_THRESH_OTSU);
	//delete_jut(img, dst, 1, 1, 0);//去除二值图像边缘的突出部  
	
	//Mat dst2;
	/*dst = thinImage(img);
	imshow("thinImage", dst);*/
	////平滑处理
	//imageblur(img, dst, Size(10, 10), thresValue);
	//imshow("【效果图】膨胀操作", dst2);
	//二值化的图像
	cvNamedWindow("Threshold", 0);
	cvShowImage("Threshold", src);

	RemoveSmallRegion(img, dst, 20, 1, 1);
	RemoveSmallRegion(img, dst, 20, 0, 0);
	cvNamedWindow("DeNoisy", 0);
	cvShowImage("DeNoisy", src);
	/*test thinning_sister*************************************/
	//imshow("Mat Img", dst);//得到的是二值化的图
						  
	//ThingSister
	//Mat dst_thin = Xihua(img, arrayXihua);
	//delete_jut(img, dst, 1, 1, 0);
	//平滑处理
	//imageblur(img, dst, Size(10, 10), thresValue);
	Mat dst_thin;
	/*Mat dst_thin=Xihua(dst, arrayXihua);
	
	
	imshow("thinning_sister", dst_thin);*/
	
	/*test-end************************************************/





	//FillInternalContours(src, 20);//轮廓填充
	//cvNamedWindow("Contours");
	//cvShowImage("Contours", src);

	//tuchubuixaochu








	//动态创建一个长度为sizeof(char) * src->width * src->height的字符数组
	//并且初始化把数组元素都设置为0
	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	

	//case 3 用到
	Mat edges;
	MorphoFeatures morpho;
	Mat corners;


	



	//scanf("%d", &flag);
	flag = 4;
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
				imagedata[y*src->width + x] = ptr[x] > 0 ? 0 : 1;
				//经过预处理后得到待细化的图像是0、1二值图像。
			}
		}
		//ThinnerRosenfeld((unsigned char *)src->imageData, src->width, src->height);
		//ThinnerPavlidis((unsigned char *)src->imageData, src->width, src->height);
		//ThinnerHilditch((unsigned char *)src->imageData, src->width, src->height);
		//ZhangThinning(src->width, src->height, imagedata);
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
				ptr[x] = imagedata[y*src->width + x]>0 ? 0 : 255;
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
		ThiningDIBSkeleton((unsigned char *)src->imageData, src->width, src->height);
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