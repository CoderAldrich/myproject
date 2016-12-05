#pragma once

/*
功能：进程创建通知的回调函数形式定义
参数：
DWORD dwParentID     父进程的ID
DWORD dwProcessID    进程ID
LPCWSTR pszProcessPath  进程路径
BOOL bCreate           是否为创建通知  TRUE 创建通知 FALSE 进程结束通知
返回值：空

注意：此回调函数必须尽快返回，否则可能会错失其他进程创建通知，建议通过队列缓存通知数据然后依次处理
*/
typedef VOID (CALLBACK *RROCESS_CREATE_CALLBACK)(DWORD dwParentID,DWORD dwProcessID,LPCWSTR pszProcessPath,BOOL bCreate);

typedef BOOL (WINAPI *TypeSetProcessCreateCallBack)(RROCESS_CREATE_CALLBACK pCallBack);