// 验证码识别.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <atlimage.h>

int _tmain(int argc, _TCHAR* argv[])
{

	CImage img;
	HRESULT hr = img.Load(L"C:\\yanzhengma2.png");
	if ( hr == S_OK )
	{
		int nFixCount = 0;
		do
		{
			nFixCount = 0;

			for (int x = 1;x<img.GetWidth()-1;x++)
			{
				for (int y = 1;y<img.GetHeight()-1;y++)
				{
					COLORREF clrRef = img.GetPixel(x,y);
					if ( 0x00ffffff != clrRef )
					{
						int nclrCount = 0;
						for (int m = -1;m<2;m++)
						{
							for (int n = -1;n<2;n++)
							{
								if ( m==0 && n == 0 )
								{

								}
								else
								{
									COLORREF clrTmpRef = img.GetPixel(x+m,y+n);
									if (clrTmpRef != 0x00ffffff)
									//if (clrTmpRef == clrRef)
									{
										nclrCount++;
									}
								}

							}
						}

						if (nclrCount <= 2)
						{
							nFixCount++;
							img.SetPixel(x,y,0x00ffffff);
						}
					}

				}
			}
		}
		while( nFixCount > 0 );
		//while(FALSE);


		img.Save(L"C:\\result.bmp");
	}

	

	return 0;
}

