#pragma once


//扫描 遍历  目录  文件夹 

/*
返回值：是否继续扫描
*/
typedef BOOL (WINAPI *TypeFileFindCallBack)( LPCWSTR pszFileFullPath , PVOID pParam );

/*
返回值：是否扫描这个文件夹
*/
typedef BOOL (WINAPI *TypeDirectFindCallBack)( LPCWSTR pszDirFullPath , PVOID pParam );

BOOL FindPath(LPCWSTR pszFindPath,LPCWSTR pszFindFile,TypeFileFindCallBack pFileFindCallBack,PVOID pFileParam,TypeDirectFindCallBack pDirFindCallBack,PVOID pDirParam);
