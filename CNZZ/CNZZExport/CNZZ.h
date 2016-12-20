#pragma once

typedef VOID (WINAPI *TypeMsgShowCallBack)(LPCWSTR pszMsgShow);
int CNZZDataExport(LPCWSTR pchSiteId,LPCWSTR pchPassWord,LPCWSTR pszDayTime,LPCWSTR pszSavePath,TypeMsgShowCallBack pMsgShowCallback);