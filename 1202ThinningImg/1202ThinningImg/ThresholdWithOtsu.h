#pragma once
#include <cv.h>   
#include <highgui.h>   
#include <windows.h>   



// ��ȡֱ��ͼ   
// 1. pImageData   ͼ������   
// 2. nWidth       ͼ����   
// 3. nHeight      ͼ��߶�   
// 4. nWidthStep   ͼ���д�С   
// 5. pHistogram   ֱ��ͼ   
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
				pLine[i] = 0xff;
			}
			else
			{
				pLine[i] = 0x00;
			}
		}
	}
	return TRUE;
}


// ��ֵ��ͼ��ȡ��   
void Not(IplImage *edge)
{
	int i;
	int j;
	for (i = 0; i<edge->height; i++)
		for (j = 0; j<edge->width; j++)
		{
			if (edge->imageData[edge->height*i + j] == 0)
				edge->imageData[edge->height*i + j] = 1;
			else
				edge->imageData[edge->height*i + j] = 0;

		}

}

BOOL FindContours(unsigned char *pImageData, int nWidth, int nHeight, int nWidthStep)
{
	int            i = 0;
	int            j = 0;
	unsigned char *pLine[3] = { NULL, NULL, NULL };
	for (j = 1; j < nHeight - 1; j++)
	{
		pLine[0] = pImageData + nWidthStep * (j - 1);
		pLine[1] = pImageData + nWidthStep * j;
		pLine[2] = pImageData + nWidthStep * (j + 1);
		for (i = 1; i < nWidth - 1; i++)
		{
			if (pLine[0][i - 1] == 0xFF && pLine[0][i] == 0xFF && pLine[0][i + 1] == 0xFF &&
				pLine[1][i - 1] == 0xFF && pLine[1][i] == 0xFF && pLine[1][i + 1] == 0xFF &&
				pLine[2][i - 1] == 0xFF && pLine[2][i] == 0xFF && pLine[2][i + 1] == 0xFF)
			{
				pLine[0][i - 1] = 0;
			}
			else
			{
				pLine[0][i - 1] = pLine[1][i];
			}
		}
	}
	return TRUE;
}