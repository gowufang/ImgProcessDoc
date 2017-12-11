
#include <highgui.h>   
#include <windows.h>   

#include "highgui.h"
#include "cv.h"//����cvCreateStructuringElementEx();��ɫ�ռ�ת������cvCvtColor()��Ҫ
#include "cxcore.h"

#include <stdlib.h>
#include <string.h>
//                                        p3  p2  p1
//**********ʹ��zhang���п����㷨����ϸ�� p4  p   p0
//                                        p5  p6  p7
void ZhangThinning(int w, int h, unsigned char *imgBuf)
{
	int neighbor[8];

	BYTE *mark = new BYTE[w*h];
	memset(mark, 0, w*h);

	BOOL loop = TRUE;
	int x, y, k;
	int markNum = 0;

	while (loop)
	{
		loop = FALSE;

		//��һ��
		markNum = 0;
		for (y = 1; y<h - 1; y++)
		{
			for (x = 1; x<w - 1; x++)
			{
				//����1��p������ǰ����
				if (imgBuf[y*w + x] == 0) continue;

				neighbor[0] = imgBuf[y*w + x + 1];
				neighbor[1] = imgBuf[(y - 1)*w + x + 1];
				neighbor[2] = imgBuf[(y - 1)*w + x];
				neighbor[3] = imgBuf[(y - 1)*w + x - 1];
				neighbor[4] = imgBuf[y*w + x - 1];
				neighbor[5] = imgBuf[(y + 1)*w + x - 1];
				neighbor[6] = imgBuf[(y + 1)*w + x];
				neighbor[7] = imgBuf[(y + 1)*w + x + 1];

				//����2��2<=N(p��<=6
				int np = (neighbor[0] + neighbor[1] + neighbor[2] + neighbor[3] + neighbor[4] + neighbor[5] + neighbor[6] + neighbor[7]) / 255;
				if (np<2 || np>6) continue;

				//����3��S(p��=1
				int sp = 0;
				for (int i = 1; i<8; i++)
				{
					if (neighbor[i] - neighbor[i - 1] == 255)
						sp++;
				}
				if (neighbor[0] - neighbor[7] == 255)
					sp++;
				if (sp != 1) continue;

				//����4��p2*p0*p6=0
				if (neighbor[2] & neighbor[0] & neighbor[6] != 0)
					continue;
				//����5��p0*p6*p4=0
				if (neighbor[0] & neighbor[6] & neighbor[4] != 0)
					continue;


				//���ɾ��
				mark[w*y + x] = 1;
				markNum++;
				loop = TRUE;
			}
		}

		//�����ɾ���ĵ���Ϊ����ɫ
		if (markNum>0)
		{
			for (y = 0; y<h; y++)
			{
				for (x = 0; x<w; x++)
				{
					k = y*w + x;
					if (mark[k] == 1)
					{
						imgBuf[k] = 0;
					}
				}
			}
		}


		//�ڶ���
		markNum = 0;
		for (y = 1; y<h - 1; y++)
		{
			for (x = 1; x<w - 1; x++)
			{
				//����1��p������ǰ����
				if (imgBuf[y*w + x] == 0) continue;

				neighbor[0] = imgBuf[y*w + x + 1];
				neighbor[1] = imgBuf[(y - 1)*w + x + 1];
				neighbor[2] = imgBuf[(y - 1)*w + x];
				neighbor[3] = imgBuf[(y - 1)*w + x - 1];
				neighbor[4] = imgBuf[y*w + x - 1];
				neighbor[5] = imgBuf[(y + 1)*w + x - 1];
				neighbor[6] = imgBuf[(y + 1)*w + x];
				neighbor[7] = imgBuf[(y + 1)*w + x + 1];

				//����2��<=N(p)<=6
				int np = (neighbor[0] + neighbor[1] + neighbor[2] + neighbor[3] + neighbor[4] + neighbor[5] + neighbor[6] + neighbor[7]) / 255;
				if (np<2 || np>6) continue;

				//����3��S(p)=1
				int sp = 0;
				for (int i = 1; i<8; i++)
				{
					if (neighbor[i] - neighbor[i - 1] == 255)
						sp++;
				}
				if (neighbor[0] - neighbor[7] == 255)
					sp++;
				if (sp != 1) continue;

				//����4��p2*p0*p4==0
				if (neighbor[2] & neighbor[0] & neighbor[4] != 0)
					continue;
				//����5��p2*p6*p4==0
				if (neighbor[2] & neighbor[6] & neighbor[4] != 0)
					continue;

				//���ɾ��
				mark[w*y + x] = 1;
				markNum++;
				loop = TRUE;
			}
		}

		//�����ɾ���ĵ���Ϊ����ɫ
		for (y = 0; y<h; y++)
		{
			for (x = 0; x<w; x++)
			{
				k = y*w + x;
				if (mark[k] == 1)
				{
					imgBuf[k] = 0;
				}
			}
		}

	}
}