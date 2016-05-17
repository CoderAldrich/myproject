#include "stdafx.h"
#include <atlstr.h>
#include <afxinet.h>

CString GetWebContext(CString strUrl,CString strFileName,BOOL *pbSucess)
{
#ifdef _DEBUG
	//OutputDebugStringW(url+TEXT("\n"));
#endif
	CString strFileUrl;
	strFileUrl=strUrl;

	CInternetSession sess;
	CHttpFile* pHttpFile       = NULL;
	CHttpConnection* pHttpConn = NULL;
	DWORD dwType = 0;
	CString sServer = _T("");
	CString sObject = _T("");
	INTERNET_PORT wPort = 0;
	ULONGLONG LastRecvSize=0;
	try
	{
		BOOL  bResult = AfxParseURL(strFileUrl, dwType, sServer, sObject, wPort);

		if (!bResult)
		{
			goto error;
		}
		pHttpConn = sess.GetHttpConnection(sServer, wPort);

		if (!pHttpConn)
		{
			goto error;
		}

		pHttpFile=pHttpConn->OpenRequest(CHttpConnection::HTTP_VERB_GET, sObject);

		if(!pHttpFile)
		{
			goto error;
		}
		if(!(pHttpFile->SendRequest()))
		{
			goto error;
		}
		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		bool bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

		char strarray[1024];
		if( !(bSuccess && dwStatus>= 200&& dwStatus<300))
		{
			goto error;
		}
		int nRecvLen=0;
		CString strRecvWebContext;

		if (strFileName.IsEmpty())
		{
			do 
			{
				char buffer[10240]={0};
				nRecvLen=pHttpFile->Read(buffer,10220);
				strRecvWebContext+=buffer;

			} while (nRecvLen > 0);
		}
		else if(!PathFileExistsW(strFileName))
		{
			CFile writefile;
			writefile.Open(strFileName,CFile::modeCreate|CFile::modeWrite);
			do
			{
				char buffer[10240]={0};
				nRecvLen=pHttpFile->Read(buffer,10240);
				writefile.Write(buffer,nRecvLen);
			} while (nRecvLen > 0);
			writefile.Close();

		}
		pHttpFile->Close();
		delete pHttpFile;
		pHttpConn->Close();
		delete pHttpConn;
		sess.Close();

		if(pbSucess)
			*pbSucess = TRUE;

		return strRecvWebContext;
	}
	catch (...)
	{
		goto error;
	}
	if(pbSucess)
		*pbSucess = FALSE;
	return TEXT("");
error:
	if(pbSucess)
		*pbSucess = FALSE;
	return TEXT("");
}