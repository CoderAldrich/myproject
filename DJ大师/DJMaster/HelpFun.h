#pragma once


//ɨ�� ����  Ŀ¼  �ļ��� 

/*
����ֵ���Ƿ����ɨ��
*/
typedef BOOL (WINAPI *TypeFileFindCallBack)( LPCWSTR pszFileFullPath , PVOID pParam );

/*
����ֵ���Ƿ�ɨ������ļ���
*/
typedef BOOL (WINAPI *TypeDirectFindCallBack)( LPCWSTR pszDirFullPath , PVOID pParam );

BOOL FindPath(LPCWSTR pszFindPath,LPCWSTR pszFindFile,TypeFileFindCallBack pFileFindCallBack,PVOID pFileParam,TypeDirectFindCallBack pDirFindCallBack,PVOID pDirParam);
