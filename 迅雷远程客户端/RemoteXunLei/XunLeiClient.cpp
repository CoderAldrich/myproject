#include "stdafx.h"
#include "XunLeiClient.h"

#include "PublicFun.h"

#include ".\json\json.h"
#ifdef DEBUG
#pragma comment(lib,"json_vc71_libmtd.lib")
#else
#pragma comment(lib,"json_vc71_libmt.lib")
#endif

BOOL XunLeiCheckLogin(CString &strErrorMsg)
{
	BOOL bLogin = FALSE;
	CStringA strData;
	strData = HttpQueryData( L"http://stat.login.xunlei.com:1800/report?cachetime=1473226733361" );
	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(strData.GetBuffer(), root))   
	{
		int nRes = 0;
		nRes = root["rtn"].asInt();
		if ( nRes == 0 )
		{
			bLogin = TRUE;
		}

		strErrorMsg = root["msg"].asCString();
	}

	return bLogin;
}

BOOL  XunLeiLongin( LPCWSTR pszUserName , LPCWSTR pszPassWord ,CString &strErrorMsg )
{
	HttpQueryData( L"http://yuancheng.xunlei.com/login.html" );
	HttpQueryData( L"https://login.xunlei.com/risk?cmd=report" );
	HttpQueryData( L"https://login.xunlei.com/check/?u=gaozan198912&business_type=113&v=101&cachetime=1473226725725&" );


	CStringA straPostData;
	straPostData.Format("p=%s&u=%s&verifycode=----&login_enable=0&business_type=113&v=101&cachetime=1473226733018",CStringA(pszPassWord),CStringA(pszUserName));
	HttpQueryData( L"https://login.xunlei.com/sec2login/",straPostData.GetBuffer(),straPostData.GetLength());

	return XunLeiCheckLogin(strErrorMsg);
}

int XunLeiQueryDownloaders( CXunLeiDownloader **pArray,int nMaxCount )
{
	int nCurCount = 0;

	CStringA strData;

	strData = HttpQueryData( L"http://homecloud.yuancheng.xunlei.com/listPeer?type=0" );

	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(strData.GetBuffer(), root))   
	{
		int nRes = 0;
		nRes = root["rtn"].asInt();
		if ( nRes == 0 )
		{
			Json::Value val_array = root["peerList"];
			int iSize = val_array.size();
			for ( int nIndex = 0;nIndex < iSize;++ nIndex )
			{
				CString strName = UTF8ToUnicode(val_array[nIndex]["name"].asCString());
				BOOL bOnLine  = val_array[nIndex]["online"].asBool(); 
				CStringA strPath = val_array[nIndex]["path_list"].asCString(); 
				CStringA strPid = val_array[nIndex]["pid"].asCString();

				pArray[nCurCount] = new CXunLeiDownloader(strName,CString(strPid));

				nCurCount++;

			}
		}

	}

	return nCurCount;
}

BOOL ParseItemsData( LPCSTR pszData )
{
	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(pszData, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		int nRes = 0;
		nRes = root["rtn"].asInt();
		if ( nRes == 0 )
		{
			int nCompleteNum = root["completeNum"].asInt();
			int ndlNum = root["dlNum"].asInt();
			int nrecycleNum = root["recycleNum"].asInt();
			int serverFailNum = root["serverFailNum"].asInt();

			Json::Value val_array = root["tasks"];
			int iSize = val_array.size();
			for ( int nIndex = 0;nIndex < iSize;++ nIndex )
			{
				CString strName = UTF8ToUnicode(val_array[nIndex]["name"].asCString());
				CStringA strUrl = val_array[nIndex]["url"].asCString(); 
				int      nProcess = val_array[nIndex]["progress"].asInt();
				long     lSpeed = val_array[nIndex]["speed"].asInt64();
				int      nStatus = val_array[nIndex]["state"].asInt();
				int a=0;
			}
		}

	}  

	return FALSE;
}

VOID XunLeiQueryItems( LPCWSTR pszDownloaderId , DOWNLOAD_ITEM_TYPE Type )
{
	CStringA strData;

	CString strTest;
	strTest.Format(L"http://homecloud.yuancheng.xunlei.com/list?pid=%s&type=%d&pos=0&number=100&needUrl=1&v=2&ct=0",pszDownloaderId,Type);

	strData = HttpQueryData( strTest );

	ParseItemsData(CStringA(strData));
}