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

int _tmain(int argc, _TCHAR* argv[])
{

	LPCWSTR pszSiteId=L"1260928318";
	LPCWSTR pszPassWord=L"123456";
	LPCWSTR pszStartTime=L"2016-12-18";
	LPCWSTR pszEndTime=L"2016-12-18";

	CString strData;
	CString strPostUrl;

	strPostUrl.Format(L"http://new.cnzz.com/v1/login.php?siteid=%s",pszSiteId);
	strData =  HttpQueryData(strPostUrl);
	
	CStringA strPostData;
	strPostData.Format("password=%s",CStringA(pszPassWord).GetBuffer());

	strPostUrl.Format(L"http://new.cnzz.com/v1/login.php?t=login&siteid=%s",pszSiteId);

	HttpQueryData(strPostUrl,NULL,NULL,NULL,strPostData.GetBuffer(),strPostData.GetLength());

	
	HANDLE hFile = CreateFile(L"C:\\CNZZData.csv",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		int nTotalCount = 0;
		int nPageIndex=0;
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
				,CString(pszSiteId).GetBuffer()
				,pszStartTime
				,pszEndTime
				,GetCurrMSForMe()
				);

			CStringA strJsonData;
			strJsonData = HttpQueryData(strDataQueryUrl);
			
			if (strJsonData.GetLength() == 0)
			{
				break;
			}

			Json::Reader reader;  
			Json::Value root;  
			if (reader.parse(strJsonData.GetBuffer(), root))   
			{
				if ( FALSE == JsonCheckNodes(&root,"data","detailPvList","items",NULL) )
				{
					break;
				}

				Json::Value val_array = root["data"]["detailPvList"]["items"];

				int iSize = val_array.size();
				for ( int nIndex = 0;nIndex < iSize;++ nIndex )
				{
					CStringA strReferer;
					CStringA strViewPage;
					strReferer = val_array[nIndex]["referer"].asCString();
					strViewPage = val_array[nIndex]["resource"].asCString();

					nTotalCount++;

					CStringA strIndex;
					strIndex.Format("%d",nTotalCount);
					
					WriteStringToFile(hFile,"\"");
					WriteStringToFile(hFile,strIndex);
					WriteStringToFile(hFile,"\",\"");
					WriteStringToFile(hFile,strReferer);
					WriteStringToFile(hFile,"\",\"");
					WriteStringToFile(hFile,strViewPage);
					WriteStringToFile(hFile,"\"\r\n");

					CStringA strPageIndex;
					strPageIndex.Format("%d",nPageIndex);
					printf(strPageIndex+" "+strIndex+" "+strReferer+" "+strViewPage+"\n");

				}

			}
			else
			{
				break;
			}
		}

		CloseHandle(hFile);
	}

	printf("提取完毕");
	getchar();

	return 0;
}

