#pragma once

typedef VOID (CALLBACK *ShellHookCallBack)(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL StartShellHook( DWORD dwHookType, ShellHookCallBack pCallBack);
