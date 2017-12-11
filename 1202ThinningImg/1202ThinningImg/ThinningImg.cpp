//�����������ͼ��ϸ���㷨

#include <highgui.h>   
#include <windows.h>   
#include "contours.h"
#include "highgui.h"
#include "cv.h"//����cvCreateStructuringElementEx();��ɫ�ռ�ת������cvCvtColor()��Ҫ
#include "cxcore.h"
#include"opencv2/imgproc/imgproc.hpp"  
#include <stdlib.h>
#include <string.h>
#include "Thinning.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
//�����������ϸ��ϸ���㷨
//���ܣ���ͼ�����ϸ��
//������lpDIBBits������ͼ���һά����
//      lWidth��ͼ��߶�
//      lHeight��ͼ����


//                           ����ͼ���һά����      ͼ����     ͼ��߶�
bool ThiningDIBSkeleton(unsigned char* imagedata, int lWidth, int lHeight)
{
	/* 	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	*/
	/*
	deletemark[256]Ϊǰ�˾�8�����ܽ���Ƿ���Ա�ɾ����256�����
	������ɾ����0����ʾ���ܱ�ɾ������1����ʾ
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
	};//������

	  //ѭ������
	  //long i;
	  //long j;


	unsigned char p0, p1, p2, p3, p4, p5, p6, p7;

	unsigned char *pmid, *vergeMark;    // pmid ����ָ���ֵͼ��  vergeMark����ָ�����Ƿ�Ϊ��Ե

	unsigned char sum;

	bool bStart = true;

	//��̬����һ������Ϊsizeof(char) * src->width * src->height������
	//���ҳ�ʼ��������Ԫ�ض�����Ϊ0
	//������imagedata����һ��
	long lLength;
	lLength = lWidth * lHeight; //���鳤��
	unsigned char *pTemp = new uchar[sizeof(unsigned char) * lLength]();

	//    P0 P1 P2
	//    P7    P3
	//    P6 P5 P4

	while (bStart)
	{
		bStart = false;
		//ÿ��ѭ��pTemp����Ԫ�ض���0
		memset(pTemp, 0, lLength);//��ʼ��pTempָ���lLength�ռ�ȫ����ʼ��Ϊ0

								  /*---------------�������Ե��----------------*/
		pmid = (unsigned char *)imagedata + lWidth + 1; // pmid ����ָ���ֵͼ�����ĵڶ���	  +1 ��ʾ������ͼ���һ��
		vergeMark = (unsigned char *)pTemp + lWidth + 1; //pmidtemp����ָ�����Ƿ�Ϊ��Ե ����Ǳ�Ե�� ������Ϊ1

		for (int i = 1; i < lHeight - 1; i++)  //lHeight -1��ʾ������ͼ�����һ�� ��һ��
		{
			for (int j = 1; j < lWidth - 1; j++)  //lWidth - 1��ʾ������ͼ�����һ��
			{
				//ͼ���Ѿ���01���������0���ǲ�����
				if (*pmid == 0)
				{
					pmid++;
					vergeMark++;
					continue;
				}
				//�����1�������ǿ��ǵĵ� ������Ҫ����Χ8�������ж��Ƿ��Ե
				p0 = *(pmid - lWidth - 1);
				p1 = *(pmid - lWidth);                  //    P0 P1 P2
				p2 = *(pmid + 1 - lWidth);              //    P7    P3
				p3 = *(pmid + 1);						//    P6 P5 P4
				p4 = *(pmid + lWidth + 1);
				p5 = *(pmid + lWidth);
				p6 = *(pmid + lWidth - 1);
				p7 = *(pmid - 1);
				//p0--��---p7��ֵ����0����1
				sum = p0 & p1 & p2 & p3 & p4 & p5 & p6 & p7;//����Ǳ�Ե���϶���Χ��P0P1P2P3P4P5P6P7 ��һ��������һ��Ϊ0
				if (sum == 0)
				{
					*vergeMark = 1;// ��Ǳ�Ե
				}

				pmid++;
				vergeMark++;
			}
			pmid++;//����ͼ�����һ�У�������
			vergeMark++;

			pmid++;//����ͼ���һ�У�������
			vergeMark++;
		}

		/*---------------���ű�Ե���ڿ�ʼɾ��----------------*/
		pmid = (unsigned char *)imagedata + lWidth + 1;
		vergeMark = (unsigned char *)pTemp + lWidth + 1;

		for (long i = 1; i < lHeight - 1; i++)   // ������ͼ���һ�� ��һ�� ���һ�� ���һ��
		{
			for (long j = 1; j < lWidth - 1; j++)
			{
				//*vergeMark=0��ʾ����㲻�Ǳ�Ե����������ɾ��
				if (*vergeMark == 0)
				{
					pmid++;
					vergeMark++;

					continue;
				}
				//�����1�������ǿ��ǵĵ� ������Ҫ����Χ8�������ж�
				//�ж�һ�����Ƿ���ȥ��, Ҫ�������İ˸����ڵ��������ж�
				p0 = *(pmid - lWidth - 1);
				p1 = *(pmid - lWidth);                  //    P0 P1 P2
				p2 = *(pmid - lWidth + 1);             //    P7    P3
				p3 = *(pmid + 1);					    //    P6 P5 P4
				p4 = *(pmid + lWidth + 1);
				p5 = *(pmid + lWidth);
				p6 = *(pmid + lWidth - 1);
				p7 = *(pmid - 1);


				/*�������İ˸����ڵ������γɵ����������ɾ������
				*
				* ����Ԥ�����õ���ϸ����ͼ����0��1��ֵͼ��
				* ����ֵΪ1������Ҫϸ���Ĳ��֣�����ֵΪ0���Ǳ�������
				*
				* ������������㷨����"����һ�����ж�����"����������һ�ű�
				* ��Ϊһ�����ص�8���������ǿ�����8λ�����Ʊ�ʾ������256�п��������
				* ��ˣ�������Ĵ�Сһ��Ϊ256��
				* ��������ֵ��Ӧ���е�λ�ô�0��1��
				*                  ��ǰ���ز���ɾ����Ϊ0��
				*                  ����ɾ����Ϊ1��
				*
				* Ȼ�����Ҫϸ���ĵ�İ˸���������������������ѯ��
				* ������Ԫ����1����ɾ���õ㣨��Ϊ������
				* ����0������
				*
				*
				* ----------------------------------------
				* |p7 | p6 | p5 | p4 | p3 | p2 | p1 | p0 |---����ֵ��Ӧ���е�λ��-----������256�п������
				* ----------------------------------------
				��λ��1 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1,
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
				1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0��λ��256��
				*/
				//��ʱp0--��---p7��ֵ����0����1
				p1 *= 2;
				p2 *= 4;
				p3 *= 8;
				p4 *= 16;
				p5 *= 32;
				p6 *= 64;
				p7 *= 128;

				sum = p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7;
				//  sum = p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7;
				if (deletemark[sum] == 1)//����ɾ��
				{
					*pmid = 0;
					bStart = true;      //  ��������ɨ�������ϸ��
				}
				pmid++;
				vergeMark++;
			}

			pmid++;//����ͼ�����һ�У�������
			vergeMark++;

			pmid++;//����ͼ���һ�У�������
			vergeMark++;
		}
	}

	//ѭ������˵��û�п��Լ���ϸ���ĵ��ˣ���Ϊϸ������
	delete[]pTemp;
	return true;
}
BOOL GetHistogram(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep,
	int *pHistogram)
{
	int            i = 0;
	int            j = 0;
	unsigned char *pLine = NULL;
	// ���ֱ��ͼ   
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

// ���ȡ��ֵ   
// 1. pImageData   ͼ������   
// 2. nWidth       ͼ����   
// 3. nHeight      ͼ��߶�   
// 4. nWidthStep   ͼ���д�С   
// ����������ֵ   
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
	// ��ȡֱ��ͼ   
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



// ��ֵ��   
// 1. pImageData   ͼ������   
// 2. nWidth       ͼ����   
// 3. nHeight      ͼ��߶�   
// 4. nWidthStep   ͼ���д�С   
// 5. nThreshold   ��ֵ   
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
//Pavlidisϸ���㷨
//���ܣ���ͼ�����ϸ��
//������image������ͼ���һά����
//      lx��ͼ����
//      ly��ͼ��߶�
//      �޷���ֵ
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
//Rosenfeldϸ���㷨
//���ܣ���ͼ�����ϸ��
//������image������ͼ���һά����
//      lx��ͼ����
//      ly��ͼ��߶�
//      �޷���ֵ
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
//�����������ϸ��ϸ���㷨
//���ܣ���ͼ�����ϸ��
//������lpDIBBits������ͼ���һά����
//      lWidth��ͼ��߶�
//      lHeight��ͼ����
//      �޷���ֵ
BOOL WINAPI ThiningDIBSkeleton(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{
	//ѭ������
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
	};//������

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

		//�������Ե��(����)
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

		//���ڿ�ʼ����ɾ��
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

//ȥ����ֵͼ���Ե��ͻ����
//uthreshold��vthreshold�ֱ��ʾͻ�����Ŀ����ֵ�͸߶���ֵ
//type����ͻ��������ɫ��0��ʾ��ɫ��1�����ɫ 
void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type)
{
	int threshold;
	src.copyTo(dst);
	int height = dst.rows;
	int width = dst.cols;
	int k;  //����ѭ���������ݵ��ⲿ
	for (int i = 0; i < height - 1; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width - 1; j++)
		{
			if (type == 0)
			{
				//������
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
				//������
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
				//������
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
				//������
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

//ͼƬ��Ե�⻬����
//size��ʾȡ��ֵ�Ĵ��ڴ�С��threshold��ʾ�Ծ�ֵͼ����ж�ֵ������ֵ
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
	//����ԭͼ��
	IplImage* src = cvLoadImage("06100018/00000115(534,694,658,828).jpg", 0);
	


	//---------------------------Ԥ�����ֵ��------------------------------------
	/*��ԭͼ����ж�ֵ��  ��ֵ
	cvThreshold(src, src, 100, 255, CV_THRESH_BINARY);

	ͼ���ֵ�����ǽ�ͼ���ϵ����ص�ĻҶ�ֵ����Ϊ0��255��
	Ҳ���ǽ�����ͼ����ֳ����Եĺڰ�Ч��
	CV_THRESH_BINARY  ��˼Ϊ�� if src(x,y) > 100
	src(x,y)=255
	else
	src(x,y)=-0
	ԭͼ����0��255---->0��255
	*/

	Mat img = cvarrToMat(src);
	Mat dst = img;
	//boxFilter(img, dst, -1, Size(5, 5));
	//blur(img, dst, Size(7, 7));
	GaussianBlur(img, dst, Size(5, 5), 0, 0);
	unsigned int thresValue = Otsu((unsigned char *)src->imageData, src->width, src->height, src->widthStep);

	Thresholding((unsigned char *)src->imageData, src->width, src->height, src->widthStep,
		thresValue);



	//delete_jut(img, dst, 3, 3, 0);//ȥ����ֵͼ���Ե��ͻ����  
	//

	////ƽ������
	//imageblur(img, dst, Size(10, 10), thresValue);
	
	//��ֵ����ͼ��
	cvNamedWindow("Threshold", 0);
	cvShowImage("Threshold", src);






	//FillInternalContours(src, 20);//�������
	//cvNamedWindow("Contours");
	//cvShowImage("Contours", src);

	//tuchubuixaochu









	//��̬����һ������Ϊsizeof(char) * src->width * src->height���ַ�����
	//���ҳ�ʼ��������Ԫ�ض�����Ϊ0
	unsigned char* imagedata;
	imagedata = new uchar[sizeof(char) * src->width * src->height]();
	






	




	//----------------------------ϸ��-----------------------------------------

	//����Ԥ�����õ���ϸ����ͼ����0��1��ֵͼ��

	//ϸ��һ�Σ���ϸ����Σ������������䣬ÿ��ϸ��Ч����һ��������ö��ϸ���޷�����ë��
	int flag = 0;
	scanf("%d", &flag);
	switch (flag)
	{
	case 0:
		//-----------------------------Ԥ����0/1��-------------------------------------
		/*
		��imagedataָ�����ԭͼ���Сһ�µ�����ռ����0��1��ֵ
		��ֵͼ��ԭ����0��255----->------0��1
		*/

		

		int x, y;
		for (y = 0; y<src->height; y++)
		{
			/*
			widthStep��ʾ�洢һ��������Ҫ���ֽ���
			ƫ�Ƶ���y��
			*/
			unsigned char* ptr = (unsigned char*)(src->imageData + y*src->widthStep);

			for (x = 0; x<src->width; x++)
			{
				/*
				�жϵ�y�е�x�е�Ԫ��ֵ�Ƿ����0
				����0ʱ�������Ӧ��λ��Ԫ������Ϊ1
				������0������Ϊ0

				imagedata�������治��0��ʱ1
				*/
				imagedata[y*src->width + x] = ptr[x] > 0 ? 1 : 0;
				//����Ԥ�����õ���ϸ����ͼ����0��1��ֵͼ��
			}
		}

		ThiningDIBSkeleton(imagedata, src->width, src->height);
		//--------------------��ϸ�����0/1ͼ��ֵ��(0/255)-----------------------------
		/*
		��ptrָ�����ԭͼ���Сһ�µ�����ռ����0��255��ֵ
		*/
		for (y = 0; y<src->height; y++)
		{
			/*
			widthStep��ʾ�洢һ��������Ҫ���ֽ���
			ƫ�Ƶ���y��
			*/
			unsigned char* ptr = (unsigned char*)(src->imageData + y*src->widthStep);

			for (x = 0; x<src->width; x++)//width:������
			{
				/*
				�жϵ�y�е�x�е�Ԫ��ֵ�Ƿ����0
				����0ʱ�������Ӧ��λ��Ԫ������Ϊ1
				������0������Ϊ0

				ptrָ��Ŀռ��ֵ����0����255
				Ҳ����ϸ��֮���ٴα�Ϊ��ֵͼ
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