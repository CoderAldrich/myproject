// CNZZ.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <Windows.h>
#include "WininetHelper.h"

#include ".\json\json.h"
#ifdef DEBUG
#pragma comment(lib,"json_vc71_libmtd.lib")
#else
#pragma comment(lib,"json_vc71_libmt.lib")
#endif

#include <vector>
using namespace std;

#include "CNZZ.h"


BOOL WriteStringToFile( HANDLE hFile,LPCSTR pszText )
{
	DWORD dwWriteLen=0;
	return WriteFile(hFile,pszText,strlen(pszText),&dwWriteLen,NULL);
}

BOOL JsonCheckNodes( Json::Value *proot , ... )
{
	BOOL bAllExist = TRUE;

	Json::Value tmpValue = *proot;

	LPCSTR pchNodeName = NULL;
	va_list arg_ptr;    //第1步,定义这个指向参数列表的变量
	va_start(arg_ptr, proot);//第2步,把上面这个变量初始化.即让它指向参数列表

	do
	{
		//第3步,获取arg_ptr指向的当前参数.这个参数的类型由va_arg的第2个参数指定
		pchNodeName = va_arg(arg_ptr, LPCSTR);
		if (pchNodeName)
		{
			BOOL bCheck = FALSE;

			if (tmpValue.size() > 0 )
			{
				vector<string> names=tmpValue.getMemberNames();  
				vector<string>::const_iterator iter2;   
				for(iter2 = names.begin(); iter2 != names.end(); iter2++)  
				{  
					if ( StrCmpIA((*iter2).c_str(),pchNodeName ) == 0 )
					{
						tmpValue = tmpValue[pchNodeName];
						bCheck = TRUE;
						break;
					}
				}  
			}

			if ( FALSE ==  bCheck )
			{
				bAllExist = FALSE;
				break;
			}
		}
	}
	while( pchNodeName != NULL );
	va_end(arg_ptr);    //第4步,做一些清理工作

	return bAllExist;
}

int CNZZDataExport(LPCWSTR pchSiteId,LPCWSTR pchPassWord,LPCWSTR pszDayTime,LPCWSTR pszSavePath,TypeMsgShowCallBack pMsgShowCallback)
{
	LPCWSTR pchStartTime=pszDayTime;
	LPCWSTR pchEndTime=pszDayTime;
	CString strSaveFileName;

	strSaveFileName.Format(L"%sCNZZData_%s_%s-%s.txt",CString(pszSavePath),CString(pchSiteId),CString(pchStartTime),CString(pchEndTime));

	CString strData;
	CString strPostUrl;

	strPostUrl.Format(L"http://new.cnzz.com/v1/login.php?siteid=%s",pchSiteId);
	strData =  HttpQueryData(strPostUrl);
	
	CStringA strPostData;
	strPostData.Format("password=%s",CStringA(pchPassWord).GetBuffer());

	strPostUrl.Format(L"http://new.cnzz.com/v1/login.php?t=login&siteid=%s",pchSiteId);

	HttpQueryData(strPostUrl,NULL,NULL,NULL,strPostData.GetBuffer(),strPostData.GetLength());

	
	HANDLE hFile = CreateFile(strSaveFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		int nTotalCount = 0;
		int nPageIndex=0;

		int nEmptyCount = 0;
		int nParseErrorCount = 0;
		int nDataErrorCount = 0;

		while ( TRUE )
		{
			CString strDataQueryUrl;
			strDataQueryUrl.Format(
				L"http://web.umeng.com/main.php?c=flow"
				L"&a=detail"
				L"&ajax=module%%3DfluxData_option%%3Dpv%%7Cmodule%%3DdetailPvList_currentPage%%3D%d_pageType%%3D90"
				L"&siteid=%s"
				L"&st=%s"
				L"&et=%s"
				L"&visitorType="
				L"&visitorAgent="
				L"&visitorAct="
				L"&location="
				L"&refererType="
				L"&ip="
				L"&referer="
				L"&keyword="
				L"&hour=24"
				L"&page="
				L"&cnzz_eid="
				L"&_=%I64d"
				,nPageIndex++
				,CString(pchSiteId).GetBuffer()
				,pchStartTime
				,pchEndTime
				,GetCurrMSForMe()
				);

			CStringA strJsonData;
			strJsonData = HttpQueryData(strDataQueryUrl);
			
			if (strJsonData.GetLength() == 0)
			{
				nEmptyCount++;
				if ( nEmptyCount > 3 )
				{
					break;
				}
			}
			else
			{
				nEmptyCount = 0;
			}

			Json::Reader reader;  
			Json::Value root;  
			if (reader.parse(strJsonData.GetBuffer(), root))   
			{

				nParseErrorCount = 0;

				if ( FALSE == JsonCheckNodes(&root,"data","detailPvList","items",NULL) )
				{
					nDataErrorCount++;
					if (nDataErrorCount > 3)
					{
						break;
					}
				}
				else
				{
					nDataErrorCount = 0;

					Json::Value val_array = root["data"]["detailPvList"]["items"];

					int iSize = val_array.size();
					for ( int nIndex = 0;nIndex < iSize;++ nIndex )
					{
						CStringA strTime;
						CStringA strIp;
						CStringA strReferer;
						CStringA strViewPage;
						strTime = val_array[nIndex]["time"].asCString();
						strIp = val_array[nIndex]["ip"].asCString();
						strReferer = val_array[nIndex]["referer"].asCString();
						strViewPage = val_array[nIndex]["resource"].asCString();

						nTotalCount++;

						CStringA strIndex;
						strIndex.Format("%d",nTotalCount);

// 						WriteStringToFile(hFile,"\"");
// 						WriteStringToFile(hFile,strIndex);
// 						WriteStringToFile(hFile,"\",\"");
// 						WriteStringToFile(hFile,strTime);
// 						WriteStringToFile(hFile,"\",\"");
// 						WriteStringToFile(hFile,strIp);
// 						WriteStringToFile(hFile,"\",\"");
// 						WriteStringToFile(hFile,strReferer);
// 						WriteStringToFile(hFile,"\",\"");
// 						WriteStringToFile(hFile,strViewPage);
// 						WriteStringToFile(hFile,"\"\r\n");

						WriteStringToFile(hFile,strViewPage);
						WriteStringToFile(hFile,"\r\n");


						CStringA strPageIndex;
						strPageIndex.Format("%d",nPageIndex);

						if (pMsgShowCallback)
						{
							pMsgShowCallback(CString(strPageIndex+" "+strIndex+" "+strTime+" "+strIp+" "+strReferer+" "+strViewPage));
						}
						

					}

				}



			}
			else
			{
				nParseErrorCount++;
				if ( nParseErrorCount > 3 )
				{
					break;
				}
				
			}
		}

		CloseHandle(hFile);
	}

	if (pMsgShowCallback)
	{
		pMsgShowCallback(L"全部导出成功");
	}

	return 0;
}

