#include "stdafx.h"

#include <atlimage.h>

BOOL PrintScreenToFile( RECT *prcPrint, int nZoom ,LPCWSTR pszSavePath )
{
	HDC hScreenDC = NULL;
	HDC hImageDC = NULL;
	CImage imgScreen;
	CImage imgFile;

	int nimgWidth = 0;
	int nimgHeight = 0;
	RECT  rcScreen = {0,0,0,0};
	do 
	{
		hScreenDC = GetDC(NULL);
		if ( NULL == hScreenDC)
		{
			break;
		}

		if (prcPrint)
		{
			nimgWidth = prcPrint->right - prcPrint->left;
			nimgHeight = prcPrint->bottom - prcPrint->top;

			memcpy_s(&rcScreen,sizeof(RECT),prcPrint,sizeof(RECT));
		}
		else
		{
			nimgWidth = ::GetSystemMetrics(SM_CXSCREEN);
			nimgHeight = ::GetSystemMetrics(SM_CYSCREEN);

			rcScreen.left = 0;
			rcScreen.top = 0;
			rcScreen.bottom = nimgHeight;
			rcScreen.right = nimgWidth;
		}

		nimgWidth = (int)((float)nimgWidth*(float)nZoom/(float)100);
		nimgHeight = (int)((float)nimgHeight*(float)nZoom/(float)100);

		if( FALSE == imgScreen.Create( nimgWidth , nimgHeight , 24 , 0 ) )
		{
			break;
		}

		hImageDC = imgScreen.GetDC();

		SetStretchBltMode (hImageDC,  HALFTONE);
		if( FALSE == ::StretchBlt(hImageDC,0,0,nimgWidth,nimgHeight,hScreenDC,rcScreen.left,rcScreen.top,rcScreen.right - rcScreen.left,rcScreen.bottom - rcScreen.top ,SRCCOPY))
		{
			break;
		}

		int maxY = imgScreen.GetHeight();
		int maxX = imgScreen.GetWidth();

		if( FALSE == imgFile.Create(maxX,maxY,8,0))//图像大小与imgSrc相同，每个像素占1字节  
		{
			break;
		}

		if(imgFile.IsNull())  
		{
			break;
		}

		//为imgDst构造256阶灰度调色表  
		RGBQUAD ColorTab[256];  
		for(int i=0;i<256;i++)  
		{     
			ColorTab[i].rgbBlue = ColorTab[i].rgbGreen = ColorTab[i].rgbRed = i;    
		}   
		imgFile.SetColorTable(0,256,ColorTab);        

		byte* pDataSrc = (byte*)imgScreen.GetBits(); //获取指向图像数据的指针  
		byte* pDataDst = (byte*)imgFile.GetBits();    
		int pitchSrc = imgScreen.GetPitch(); //获取每行图像占用的字节数 +：top-down；-：bottom-up DIB  
		int pitchDst = imgFile.GetPitch();    
		int bitCountSrc = imgScreen.GetBPP()/8;  // 获取每个像素占用的字节数  
		int bitCountDst = imgFile.GetBPP()/8;  

		if((bitCountSrc!=3)||(bitCountDst!=1))  
			return FALSE;  

		int tmpR,tmpG,tmpB,avg;  

		for(int i=0;i<maxX;i++)  
		{  
			for(int j=0;j<maxY;j++)  
			{  
				tmpR = *(pDataSrc+pitchSrc*j+i*bitCountSrc);  
				tmpG = *(pDataSrc+pitchSrc*j+i*bitCountSrc+1);  
				tmpB = *(pDataSrc+pitchSrc*j+i*bitCountSrc+2);  
				avg = (int)(tmpR+tmpG+tmpB)/3;  
				*(pDataDst+pitchDst*j+i*bitCountDst) = avg;  
			}  
		}  

		imgFile.Save(pszSavePath/*,Gdiplus::ImageFormatJPEG*/);

	} while (FALSE);

	if (hImageDC)
	{
		imgScreen.ReleaseDC();
	}
	if (hScreenDC)
	{
		ReleaseDC(NULL,hScreenDC);
	}

	return FALSE;
}