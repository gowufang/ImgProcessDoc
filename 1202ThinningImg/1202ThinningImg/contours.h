
#include "cxcore.h" 
#include "cv.h" 
#include "highgui.h"


// ��������� 
// ����: 
// 1. pBinary: �����ֵͼ�񣬵�ͨ����λ��IPL_DEPTH_8U��
// 2. dAreaThre: �����ֵ�������������С�ڵ���dAreaThreʱ��������䡣 
void FillInternalContours(IplImage *pBinary, double dAreaThre)
{
	double dConArea;
	CvSeq *pContour = NULL;
	CvSeq *pConInner = NULL;
	CvMemStorage *pStorage = NULL;
	// ִ������ 
	if (pBinary)
	{
		// ������������ 
		pStorage = cvCreateMemStorage(0);
		cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// ����������� 
		cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
		// ������ѭ�� 
		int wai = 0;
		int nei = 0;
		for (; pContour != NULL; pContour = pContour->h_next)
		{
			wai++;
			// ������ѭ�� 
			for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
			{
				nei++;
				// ��������� 
				dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
				printf("%f\n", dConArea);
				if (dConArea <= dAreaThre)
				{
					cvDrawContours(pBinary, pConInner, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, CV_FILLED, 8, cvPoint(0, 0));
				}
			}
		}
		printf("wai = %d, nei = %d", wai, nei);
		cvReleaseMemStorage(&pStorage);
		pStorage = NULL;
	}
}
