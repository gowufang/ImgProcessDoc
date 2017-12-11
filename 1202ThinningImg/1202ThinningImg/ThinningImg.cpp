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
using namespace cv;
//基于索引表的细化细化算法
//功能：对图象进行细化
//参数：lpDIBBits：代表图象的一维数组
//      lWidth：图象高度
//      lHeight：图象宽度


//                           代表图象的一维数组      图象宽度     图象高度
bool ThiningDIBSkeleton(unsigned char* imagedata, int lWidth, int lHeight)
{
	/* 	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	*/
	/*
	deletemark[256]为前人据8领域总结的是否可以被删除的256种情况
	不可以删除用0来表示，能被删除的用1来表示
	*/
	unsigned char deletemark[256] = {
		0, 0, 0, 0, 0, 0, 0, 1,     0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 1, 1, 1, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     1, 0, 0, 0, 1, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     1, 0, 1, 1, 1, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,     1, 0, 0, 0, 1, 0, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0,     1, 0, 1, 1, 1, 0, 1, 1,
		0, 0, 1, 1, 0, 0, 1, 1,     0, 0, 0, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 1, 0, 0, 1, 1,
		1, 1, 0, 1, 0, 0, 0, 1,     0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 0, 1, 0, 0, 0, 1,     1, 1, 0, 0, 1, 0, 0, 0,
		0, 1, 1, 1, 0, 0, 1, 1,     0, 0, 0, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 1, 1, 1,
		1, 1, 1, 1, 0, 0, 1, 1,     1, 1, 0, 0, 1, 1, 0, 0,
		1, 1, 1, 1, 0, 0, 1, 1,     1, 1, 0, 0, 1, 1, 0, 0
	};//索引表

	  //循环变量
	  //long i;
	  //long j;


	unsigned char p0, p1, p2, p3, p4, p5, p6, p7;

	unsigned char *pmid, *vergeMark;    // pmid 用来指向二值图像  vergeMark用来指向存放是否为边缘

	unsigned char sum;

	bool bStart = true;

	//动态创建一个长度为sizeof(char) * src->width * src->height的数组
	//并且初始化把数组元素都设置为0
	//长度与imagedata长度一样
	long lLength;
	lLength = lWidth * lHeight; //数组长度
	unsigned char *pTemp = new uchar[sizeof(unsigned char) * lLength]();

	//    P0 P1 P2
	//    P7    P3
	//    P6 P5 P4

	while (bStart)
	{
		bStart = false;
		//每次循环pTemp所有元素都置0
		memset(pTemp, 0, lLength);//初始化pTemp指向的lLength空间全部初始化为0

								  /*---------------首先求边缘点----------------*/
		pmid = (unsigned char *)imagedata + lWidth + 1; // pmid 用来指向二值图像矩阵的第二行	  +1 表示不考虑图像第一行
		vergeMark = (unsigned char *)pTemp + lWidth + 1; //pmidtemp用来指向存放是否为边缘 如果是边缘点 则将其设为1

		for (int i = 1; i < lHeight - 1; i++)  //lHeight -1表示不考虑图像最后一行 第一列
		{
			for (int j = 1; j < lWidth - 1; j++)  //lWidth - 1表示不考虑图像最后一列
			{
				//图像已经是01化，如果是0我们不考虑
				if (*pmid == 0)
				{
					pmid++;
					vergeMark++;
					continue;
				}
				//如果是1，是我们考虑的点 我们需要对周围8个进行判断是否边缘
				p0 = *(pmid - lWidth - 1);
				p1 = *(pmid - lWidth);                  //    P0 P1 P2
				p2 = *(pmid + 1 - lWidth);              //    P7    P3
				p3 = *(pmid + 1);						//    P6 P5 P4
				p4 = *(pmid + lWidth + 1);
				p5 = *(pmid + lWidth);
				p6 = *(pmid + lWidth - 1);
				p7 = *(pmid - 1);
				//p0--到---p7的值不是0就是1
				sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;//如果是边缘，肯定周围的P0P1P2P3P4P5P6P7 中一定至少有一个为0
				if (sum == 0)
				{
					*vergeMark = 1;// 表记边缘
				}

				pmid++;
				vergeMark++;
			}
			pmid++;//跳过图像最后一列，不考虑
			vergeMark++;

			pmid++;//跳过图像第一列，不考虑
			vergeMark++;
		}

		/*---------------沿着边缘现在开始删除----------------*/
		pmid = (unsigned char *)imagedata + lWidth + 1;
		vergeMark = (unsigned char *)pTemp + lWidth + 1;

		for (long i = 1; i < lHeight - 1; i++)   // 不考虑图像第一行 第一列 最后一行 最后一列
		{
			for (long j = 1; j < lWidth - 1; j++)
			{
				//*vergeMark=0表示这个点不是边缘，即不进行删除
				if (*vergeMark == 0)
				{
					pmid++;
					vergeMark++;

					continue;
				}
				//如果是1，是我们考虑的点 我们需要对周围8个进行判断
				//判断一个点是否能去掉, 要根据它的八个相邻点的情况来判断
				p0 = *(pmid - lWidth - 1);
				p1 = *(pmid - lWidth);                  //    P0 P1 P2
				p2 = *(pmid - lWidth + 1);             //    P7    P3
				p3 = *(pmid + 1);					    //    P6 P5 P4
				p4 = *(pmid + lWidth + 1);
				p5 = *(pmid + lWidth);
				p6 = *(pmid + lWidth - 1);
				p7 = *(pmid - 1);


				/*根据它的八个相邻点的情况形成的索引表进行删除操作
				*
				* 经过预处理后得到待细化的图像是0、1二值图像。
				* 像素值为1的是需要细化的部分，像素值为0的是背景区域。
				*
				* 基于索引表的算法就是"依据一定的判断依据"，所做出的一张表，
				* 因为一个像素的8个邻域，我们可以用8位二进制表示，共有256中可能情况，
				* 因此，索引表的大小一般为256。
				* 根据索引值对应表中的位置存0或1，
				*                  当前像素不能删除存为0，
				*                  可以删除存为1。
				*
				* 然后根据要细化的点的八个邻域的在索引表中情况查询，
				* 若表中元素是1，则删除该点（改为背景）
				* 若是0则保留。
				*
				*
				* ----------------------------------------
				* |p7 | p6 | p5 | p4 | p3 | p2 | p1 | p0 |---索引值对应表中的位置-----》共有256中可能情况
				* ----------------------------------------
				（位置1 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
				1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1,
				0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,
				1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0,
				0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
				1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0,
				1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0（位置256）
				*/
				//此时p0--到---p7的值不是0就是1
				p1 *= 2;
				p2 *= 4;
				p3 *= 8;
				p4 *= 16;
				p5 *= 32;
				p6 *= 64;
				p7 *= 128;

				sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
				//  sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7;
				if (deletemark[sum] == 1)//可以删除
				{
					*pmid = 0;
					bStart = true;      //  表明本次扫描进行了细化
				}
				pmid++;
				vergeMark++;
			}

			pmid++;//跳过图像最后一列，不考虑
			vergeMark++;

			pmid++;//跳过图像第一列，不考虑
			vergeMark++;
		}
	}

	//循环结束说明没有可以继续细化的点了，则为细化结束
	delete[]pTemp;
	return true;
}
BOOL GetHistogram(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep,
	int *pHistogram)
{
	int            i = 0;
	int            j = 0;
	unsigned char *pLine = NULL;
	// 清空直方图   
	memset(pHistogram, 0, sizeof(int) * 256);

	for (pLine = pImageData, j = 0; j < nHeight; j++, pLine += nWidthStep)
	{
		for (i = 0; i < nWidth; i++)
		{
			pHistogram[pLine[i]]++;
		}
	}
	return TRUE;
}

// 大津法取阈值   
// 1. pImageData   图像数据   
// 2. nWidth       图像宽度   
// 3. nHeight      图像高度   
// 4. nWidthStep   图像行大小   
// 函数返回阈值   
int Otsu(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep)
{
	int    i = 0;
	int    j = 0;
	int    nTotal = 0;
	int    nSum = 0;
	int    A = 0;
	int    B = 0;
	double u = 0;
	double v = 0;
	double dVariance = 0;
	double dMaximum = 0;
	int    nThreshold = 0;
	int    nHistogram[256];
	// 获取直方图   
	GetHistogram(pImageData, nWidth, nHeight, nWidthStep, nHistogram);
	for (i = 0; i < 256; i++)
	{
		nTotal += nHistogram[i];
		nSum += (nHistogram[i] * i);
	}
	for (j = 0; j < 256; j++)
	{
		A = 0;
		B = 0;
		for (i = 0; i < j; i++)
		{
			A += nHistogram[i];
			B += (nHistogram[i] * i);
		}
		if (A > 0)
		{
			u = B / A;
		}
		else
		{
			u = 0;
		}
		if (nTotal - A > 0)
		{
			v = (nSum - B) / (nTotal - A);
		}
		else
		{
			v = 0;
		}
		dVariance = A * (nTotal - A) * (u - v) * (u - v);
		if (dVariance > dMaximum)
		{
			dMaximum = dVariance;
			nThreshold = j;
		}
	}
	return nThreshold;
}



// 二值化   
// 1. pImageData   图像数据   
// 2. nWidth       图像宽度   
// 3. nHeight      图像高度   
// 4. nWidthStep   图像行大小   
// 5. nThreshold   阈值   
BOOL Thresholding(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep,
	unsigned int nThreshold)
{
	int            i = 0;
	int            j = 0;
	unsigned char *pLine = NULL;
	for (pLine = pImageData, j = 0; j < nHeight; j++, pLine += nWidthStep)
	{
		for (i = 0; i < nWidth; i++)
		{
			if (pLine[i] >= nThreshold)
			{
				pLine[i] = 0x00;
			}
			else
			{
				pLine[i] = 0xff;
			}
		}
	}
	return TRUE;
}





void ThinnerHilditch(void *image, unsigned long lx, unsigned long ly)
{
	char *f, *g;
	char n[10];
	unsigned int counter;
	short k, shori, xx, nrn;
	unsigned long i, j;
	long kk, kk11, kk12, kk13, kk21, kk22, kk23, kk31, kk32, kk33, size;
	size = (long)lx * (long)ly;
	g = (char *)malloc(size);

	if (g == NULL)
	{
		printf("error in allocating memory!\n");
		return;
	}

	f = (char *)image;
	for (i = 0; i<lx; i++)
	{
		for (j = 0; j<ly; j++)
		{
			kk = i*ly + j;
			if (f[kk] != 0)
			{
				f[kk] = 1;
				g[kk] = f[kk];
			}
		}
	}

	counter = 1;

	do
	{
		printf("%4d*", counter);
		counter++;
		shori = 0;

		for (i = 0; i<lx; i++)
		{
			for (j = 0; j<ly; j++)
			{
				kk = i*ly + j;
				if (f[kk]<0)
					f[kk] = 0;
				g[kk] = f[kk];
			}
		}

		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;

				if (f[kk] != 1)
					continue;

				kk11 = (i - 1)*ly + j - 1;
				kk12 = kk11 + 1;
				kk13 = kk12 + 1;
				kk21 = i*ly + j - 1;
				kk22 = kk21 + 1;
				kk23 = kk22 + 1;
				kk31 = (i + 1)*ly + j - 1;
				kk32 = kk31 + 1;
				kk33 = kk32 + 1;

				if ((g[kk12] && g[kk21] && g[kk23] && g[kk32]) != 0)
					continue;

				nrn = g[kk11] + g[kk12] + g[kk13] + g[kk21] + g[kk23] +
					g[kk31] + g[kk32] + g[kk33];

				if (nrn <= 1)
				{
					f[kk22] = 2;
					continue;
				}

				n[4] = f[kk11];
				n[3] = f[kk12];
				n[2] = f[kk13];
				n[5] = f[kk21];
				n[1] = f[kk23];
				n[6] = f[kk31];
				n[7] = f[kk32];
				n[8] = f[kk33];
				n[9] = n[1];
				xx = 0;

				for (k = 1; k<8; k = k + 2)
				{
					if ((!n[k]) && (n[k + 1] || n[k + 2]))
						xx++;
				}

				if (xx != 1)
				{
					f[kk22] = 2;
					continue;
				}

				if (f[kk12] == -1)
				{
					f[kk12] = 0;
					n[3] = 0;
					xx = 0;

					for (k = 1; k<8; k = k + 2)
					{
						if ((!n[k]) && (n[k + 1] || n[k + 2]))
							xx++;
					}

					if (xx != 1)
					{
						f[kk12] = -1;
						continue;
					}

					f[kk12] = -1;
					n[3] = -1;
				}

				if (f[kk21] != -1)
				{
					f[kk22] = -1;
					shori = 1;
					continue;
				}

				f[kk21] = 0;
				n[5] = 0;
				xx = 0;

				for (k = 1; k<8; k = k + 2)
				{
					if ((!n[k]) && (n[k + 1] || n[k + 2]))
					{
						xx++;
					}
				}

				if (xx == 1)
				{
					f[kk21] = -1;
					f[kk22] = -1;
					shori = 1;
				}
				else
					f[kk21] = -1;
			}
		}
	} while (shori);

	free(g);
}
/////////////////////////////////////////////////////////////////////////
//Pavlidis细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值
void ThinnerPavlidis(void *image, unsigned long lx, unsigned long ly)
{
	char erase, n[8];
	char *f;
	unsigned char bdr1, bdr2, bdr4, bdr5;
	short c, k, b;
	unsigned long i, j;
	long kk, kk1, kk2, kk3;
	f = (char*)image;

	for (i = 1; i<lx - 1; i++)
	{
		for (j = 1; j<ly - 1; j++)
		{
			kk = i*ly + j;
			if (f[kk])
				f[kk] = 1;
		}
	}

	for (i = 0, kk1 = 0, kk2 = ly - 1; i<lx; i++, kk1 += ly, kk2 += ly)
	{
		f[kk1] = 0;
		f[kk2] = 0;
	}

	for (j = 0, kk = (lx - 1)*ly; j<ly; j++, kk++)
	{
		f[j] = 0;
		f[kk] = 0;
	}

	c = 5;
	erase = 1;
	while (erase)
	{
		c++;
		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] != 1)
					continue;

				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk3 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];

				bdr1 = 0;
				for (k = 0; k<8; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
				}

				if ((bdr1 & 0252) == 0252)
					continue;
				f[kk] = 2;
				b = 0;

				for (k = 0; k <= 7; k++)
				{
					b += bdr1&(0x80 >> k);
				}

				if (b <= 1)
					f[kk] = 3;

				if ((bdr1 & 0160) != 0 && (bdr1 & 07) != 0 && (bdr1 & 0210) == 0)
					f[kk] = 3;
				else if ((bdr1 && 0301) != 0 && (bdr1 & 034) != 0 && (bdr1 & 042) == 0)
					f[kk] = 3;
				else if ((bdr1 & 0202) == 0 && (bdr1 & 01) != 0)
					f[kk] = 3;
				else if ((bdr1 & 0240) == 0 && (bdr1 & 0100) != 0)
					f[kk] = 3;
				else if ((bdr1 & 050) == 0 && (bdr1 & 020) != 0)
					f[kk] = 3;
				else if ((bdr1 & 012) == 0 && (bdr1 & 04) != 0)
					f[kk] = 3;
			}
		}

		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (!f[kk])
					continue;

				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk2 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];
				bdr1 = bdr2 = 0;

				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
					if (n[k] >= 2)
						bdr2 |= 0x80 >> k;
				}

				if (bdr1 == bdr2)
				{
					f[kk] = 4;
					continue;
				}

				if (f[kk] != 2)
					continue;

				if ((bdr2 & 0200) != 0 && (bdr1 & 010) == 0 &&
					((bdr1 & 0100) != 0 && (bdr1 & 001) != 0 ||
					((bdr1 & 0100) != 0 || (bdr1 & 001) != 0) &&
						(bdr1 & 060) != 0 && (bdr1 & 06) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 040) != 0 && (bdr1 & 02) == 0 &&
					((bdr1 & 020) != 0 && (bdr1 & 0100) != 0 ||
					((bdr1 & 020) != 0 || (bdr1 & 0100) != 0) &&
						(bdr1 & 014) != 0 && (bdr1 & 0201) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 010) != 0 && (bdr1 & 0200) == 0 &&
					((bdr1 & 04) != 0 && (bdr1 & 020) != 0 ||
					((bdr1 & 04) != 0 || (bdr1 & 020) != 0) &&
						(bdr1 & 03) != 0 && (bdr1 & 0140) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 02) != 0 && (bdr1 & 040) == 0 &&
					((bdr1 & 01) != 0 && (bdr1 & 04) != 0 ||
					((bdr1 & 01) != 0 || (bdr1 & 04) != 0) &&
						(bdr1 & 0300) != 0 && (bdr1 & 030) != 0))
				{
					f[kk] = 4;
				}
			}
		}

		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] != 2)
					continue;
				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk2 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];
				bdr4 = bdr5 = 0;
				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 4)
						bdr4 |= 0x80 >> k;
					if (n[k] >= 5)
						bdr5 |= 0x80 >> k;
				}
				if ((bdr4 & 010) == 0)
				{
					f[kk] = 5;
					continue;
				}
				if ((bdr4 & 040) == 0 && bdr5 == 0)
				{
					f[kk] = 5;
					continue;
				}
				if (f[kk] == 3 || f[kk] == 4)
					f[kk] = c;
			}
		}

		erase = 0;
		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] == 2 || f[kk] == 5)
				{
					erase = 1;
					f[kk] = 0;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//Rosenfeld细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值
void ThinnerRosenfeld(void *image, unsigned long lx, unsigned long ly)
{
	char *f, *g;
	char n[10];
	char a[5] = { 0, -1, 1, 0, 0 };
	char b[5] = { 0, 0, 0, 1, -1 };
	char nrnd, cond, n48, n26, n24, n46, n68, n82, n123, n345, n567, n781;
	short k, shori;
	unsigned long i, j;
	long ii, jj, kk, kk1, kk2, kk3, size;
	size = (long)lx * (long)ly;

	g = (char *)malloc(size);
	if (g == NULL)
	{
		printf("error in alocating mmeory!\n");
		return;
	}

	f = (char *)image;
	for (kk = 0l; kk<size; kk++)
	{
		g[kk] = f[kk];
	}

	do
	{
		shori = 0;
		for (k = 1; k <= 4; k++)
		{
			for (i = 1; i<lx - 1; i++)
			{
				ii = i + a[k];

				for (j = 1; j<ly - 1; j++)
				{
					kk = i*ly + j;

					if (!f[kk])
						continue;

					jj = j + b[k];
					kk1 = ii*ly + jj;

					if (f[kk1])
						continue;

					kk1 = kk - ly - 1;
					kk2 = kk1 + 1;
					kk3 = kk2 + 1;
					n[3] = f[kk1];
					n[2] = f[kk2];
					n[1] = f[kk3];
					kk1 = kk - 1;
					kk3 = kk + 1;
					n[4] = f[kk1];
					n[8] = f[kk3];
					kk1 = kk + ly - 1;
					kk2 = kk1 + 1;
					kk3 = kk2 + 1;
					n[5] = f[kk1];
					n[6] = f[kk2];
					n[7] = f[kk3];

					nrnd = n[1] + n[2] + n[3] + n[4]
						+ n[5] + n[6] + n[7] + n[8];
					if (nrnd <= 1)
						continue;

					cond = 0;
					n48 = n[4] + n[8];
					n26 = n[2] + n[6];
					n24 = n[2] + n[4];
					n46 = n[4] + n[6];
					n68 = n[6] + n[8];
					n82 = n[8] + n[2];
					n123 = n[1] + n[2] + n[3];
					n345 = n[3] + n[4] + n[5];
					n567 = n[5] + n[6] + n[7];
					n781 = n[7] + n[8] + n[1];

					if (n[2] == 1 && n48 == 0 && n567>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[6] == 1 && n48 == 0 && n123>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[8] == 1 && n26 == 0 && n345>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[4] == 1 && n26 == 0 && n781>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[5] == 1 && n46 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[7] == 1 && n68 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[1] == 1 && n82 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[3] == 1 && n24 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					cond = 1;
					if (!cond)
						continue;
					g[kk] = 0;
					shori = 1;
				}
			}

			for (i = 0; i<lx; i++)
			{
				for (j = 0; j<ly; j++)
				{
					kk = i*ly + j;
					f[kk] = g[kk];
				}
			}
		}
	} while (shori);

	free(g);
}

/////////////////////////////////////////////////////////////////////////
//基于索引表的细化细化算法
//功能：对图象进行细化
//参数：lpDIBBits：代表图象的一维数组
//      lWidth：图象高度
//      lHeight：图象宽度
//      无返回值
BOOL WINAPI ThiningDIBSkeleton(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{
	//循环变量
	long i;
	long j;
	long lLength;

	unsigned char deletemark[256] = {
		0,0,0,0,0,0,0,1,	0,0,1,1,0,0,1,1,
		0,0,0,0,0,0,0,0,	0,0,1,1,1,0,1,1,
		0,0,0,0,0,0,0,0,	1,0,0,0,1,0,1,1,
		0,0,0,0,0,0,0,0,	1,0,1,1,1,0,1,1,
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,	1,0,0,0,1,0,1,1,
		1,0,0,0,0,0,0,0,	1,0,1,1,1,0,1,1,
		0,0,1,1,0,0,1,1,	0,0,0,1,0,0,1,1,
		0,0,0,0,0,0,0,0,	0,0,0,1,0,0,1,1,
		1,1,0,1,0,0,0,1,	0,0,0,0,0,0,0,0,
		1,1,0,1,0,0,0,1,	1,1,0,0,1,0,0,0,
		0,1,1,1,0,0,1,1,	0,0,0,1,0,0,1,1,
		0,0,0,0,0,0,0,0,	0,0,0,0,0,1,1,1,
		1,1,1,1,0,0,1,1,	1,1,0,0,1,1,0,0,
		1,1,1,1,0,0,1,1,	1,1,0,0,1,1,0,0
	};//索引表

	unsigned char p0, p1, p2, p3, p4, p5, p6, p7;
	unsigned char *pmid, *pmidtemp;
	unsigned char sum;
	int changed;
	bool bStart = true;
	lLength = lWidth * lHeight;
	unsigned char *pTemp = (unsigned char *)malloc(sizeof(unsigned char) * lWidth * lHeight);

	//    P0 P1 P2
	//    P7    P3
	//    P6 P5 P4

	while (bStart)
	{
		bStart = false;
		changed = 0;

		//首先求边缘点(并行)
		pmid = (unsigned char *)lpDIBBits + lWidth + 1;
		memset(pTemp, (BYTE)0, lLength);
		pmidtemp = (unsigned char *)pTemp + lWidth + 1;
		for (i = 1; i < lHeight - 1; i++)
		{
			for (j = 1; j < lWidth - 1; j++)
			{
				if (*pmid == 0)
				{
					pmid++;
					pmidtemp++;
					continue;
				}

				p3 = *(pmid + 1);
				p2 = *(pmid + 1 - lWidth);
				p1 = *(pmid - lWidth);
				p0 = *(pmid - lWidth - 1);
				p7 = *(pmid - 1);
				p6 = *(pmid + lWidth - 1);
				p5 = *(pmid + lWidth);
				p4 = *(pmid + lWidth + 1);

				sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;
				if (sum == 0)
				{
					*pmidtemp = 1;
				}

				pmid++;
				pmidtemp++;
			}
			pmid++;
			pmid++;
			pmidtemp++;
			pmidtemp++;
		}

		//现在开始串行删除
		pmid = (unsigned char *)lpDIBBits + lWidth + 1;
		pmidtemp = (unsigned char *)pTemp + lWidth + 1;

		for (i = 1; i < lHeight - 1; i++)
		{
			for (j = 1; j < lWidth - 1; j++)
			{
				if (*pmidtemp == 0)
				{
					pmid++;
					pmidtemp++;
					continue;
				}

				p3 = *(pmid + 1);
				p2 = *(pmid + 1 - lWidth);
				p1 = *(pmid - lWidth);
				p0 = *(pmid - lWidth - 1);
				p7 = *(pmid - 1);
				p6 = *(pmid + lWidth - 1);
				p5 = *(pmid + lWidth);
				p4 = *(pmid + lWidth + 1);

				p1 *= 2;
				p2 *= 4;
				p3 *= 8;
				p4 *= 16;
				p5 *= 32;
				p6 *= 64;
				p7 *= 128;

				sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
				if (deletemark[sum] == 1)
				{
					*pmid = 0;
					bStart = true;
				}

				pmid++;
				pmidtemp++;
			}

			pmid++;
			pmid++;
			pmidtemp++;
			pmidtemp++;
		}
	}

	return true;
}

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
int main(int argc, char* argv[])
{
	//加载原图像
	IplImage* src = cvLoadImage("06100018/00000115(534,694,658,828).jpg", 0);
	


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

	Mat img = cvarrToMat(src);
	Mat dst = img;
	//boxFilter(img, dst, -1, Size(5, 5));
	//blur(img, dst, Size(7, 7));
	GaussianBlur(img, dst, Size(5, 5), 0, 0);
	unsigned int thresValue = Otsu((unsigned char *)src->imageData, src->width, src->height, src->widthStep);

	Thresholding((unsigned char *)src->imageData, src->width, src->height, src->widthStep,
		thresValue);



	//delete_jut(img, dst, 3, 3, 0);//去除二值图像边缘的突出部  
	//

	////平滑处理
	//imageblur(img, dst, Size(10, 10), thresValue);
	
	//二值化的图像
	cvNamedWindow("Threshold", 0);
	cvShowImage("Threshold", src);






	//FillInternalContours(src, 20);//轮廓填充
	//cvNamedWindow("Contours");
	//cvShowImage("Contours", src);

	//tuchubuixaochu









	//动态创建一个长度为sizeof(char) * src->width * src->height的字符数组
	//并且初始化把数组元素都设置为0
	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	






	




	//----------------------------细化-----------------------------------------

	//经过预处理后得到待细化的图像是0、1二值图像。

	//细化一次，和细化多次，由于索引表不变，每次细化效果都一样，想采用多次细化无法消除毛刺
	int flag = 0;
	scanf("%d", &flag);
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

				ptr指向的空间的值不是0就是255
				也就是细化之后再次变为二值图
				*/
				ptr[x] = imagedata[y*src->width + x]>0 ? 255 : 0;
			}

		}
		//-------------------------------------------------------------------------
		//ZhangThinning(src->width, src->height, (unsigned char *)src->imageData);
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