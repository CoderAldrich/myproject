#include "stdafx.h"
#include "WndProcHook.h"

namespace
{
	CRWLock::CRWLock(void)
		: _rlockCount(0),
		_st(RWLOCK_IDLE),
		_rwaitingCount(0),
		_wwaitingCount(0)
	{
		//_stLock = CreateMutex(NULL, FALSE, NULL);
		//assert(_stLock != INVALID_HANDLE_VALUE);
		InitializeCriticalSection(&_stLock);

		/*
		* ���赱ǰ�ж�������������ڵȴ�д���ͷ�,��ô��д�����ͷ�ʱ,������Щ������Ӧ���л�����ִ��.
		*/
		_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CRWLock::~CRWLock(void)
	{
		//CloseHandle(_stLock);
		DeleteCriticalSection(&_stLock);
		CloseHandle(_ev);
	}

	void CRWLock::rlock()
	{
		bool isWaitReturn = false;
		while(1)
		{
			//WaitForSingleObject(_stLock, INFINITE);
			EnterCriticalSection(&_stLock);
			if(isWaitReturn)
			{
				/*
				* �ȴ��¼�����,���¾�����.
				*/
				--_rwaitingCount;
			}

			if(_st == RWLOCK_IDLE)
			{
				/*
				* ����״̬,ֱ�ӵõ�����Ȩ
				*/
				_st = RWLOCK_R;
				_rlockCount++;
				//ReleaseMutex(_stLock);
				LeaveCriticalSection(&_stLock);
				break;
			}
			else if( _st == RWLOCK_R)
			{
				if(_wwaitingCount > 0)
				{
					/*
					* ��д�����ڵȴ�,��һ��ȴ�,��ʹд���ܻ�þ�������.
					*/
					++_rwaitingCount;
					ResetEvent(_ev);
					//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
					LeaveCriticalSection(&_stLock);

					/*
					* ��Ȼ LeaveCriticalSection() �� WaitForSingleObject() ֮����һ��ʱ�䴰��,
					* ��������windowsƽ̨���¼��ź��ǲ��ᶪʧ��,����û������.
					*/
					WaitForSingleObject(_ev, INFINITE);

					/*
					* �ȴ�����,�������Լ���.
					*/
					isWaitReturn = true;
				}
				else
				{	
					/*
					* �õ�����,����+1
					*/
					++_rlockCount;
					//ReleaseMutex(_stLock);
					LeaveCriticalSection(&_stLock);
					break;
				}
			}
			else if(_st == RWLOCK_W)
			{
				/*
				* �ȴ�д���ͷ�
				*/
				++_rwaitingCount;
				ResetEvent(_ev);
				//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
				LeaveCriticalSection(&_stLock);
				WaitForSingleObject(_ev, INFINITE);

				/*
				* �ȴ�����,�������Լ���.
				*/
				isWaitReturn = true;
			}
			else
			{
				break;
			}
		}
	}

	void CRWLock::wlock()
	{
		bool isWaitReturn = false;

		while(1)
		{
			//WaitForSingleObject(_stLock, INFINITE);
			EnterCriticalSection(&_stLock);

			if(isWaitReturn) --_wwaitingCount;

			if(_st == RWLOCK_IDLE)
			{
				_st = RWLOCK_W;
				//ReleaseMutex(_stLock);
				LeaveCriticalSection(&_stLock);
				break;
			}
			else
			{
				++_wwaitingCount;
				ResetEvent(_ev);
				//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
				LeaveCriticalSection(&_stLock);
				WaitForSingleObject(_ev, INFINITE);

				isWaitReturn = true;
			}
		}
	}

	void CRWLock::unlock()
	{
		//WaitForSingleObject(_stLock, INFINITE);
		EnterCriticalSection(&_stLock);
		if(_rlockCount > 0)
		{
			/* �������� */
			--_rlockCount;

			if( 0 == _rlockCount)
			{
				_st = RWLOCK_IDLE;

				/* �ͷ� */
				if( _wwaitingCount > 0 || _rwaitingCount > 0 )
				{
					/* 
					* ��ʱ�����������ڵȴ�,�������еȴ����߳�.(�ֶ��¼�).
					* ʹ��Щ�������¾�����.
					*/
					SetEvent(_ev);
				}
				else
				{
					/* ���� */
				}
			}
			else
			{
				/* ���ж��� */
			}
		}
		else
		{
			_st = RWLOCK_IDLE;

			/* д������ */
			if( _wwaitingCount > 0 || _rwaitingCount > 0 )
			{
				/* 
				* �����ռ�л�����_stLock�������,�����¼�,��ô���ܻ�ʹһЩ�������ܵõ���������.
				* �������unlockʱ,��һ���߳����õ���rlock����wlock.������ͷŻ�����,ֻ��֮ǰ�Ѿ��ȴ����������л�����������Ȩ.
				*/
				SetEvent(_ev);
			}
			else
			{
				/* ���� */
			}
		}
		//ReleaseMutex(_stLock);
		LeaveCriticalSection(&_stLock);
	}

	CRWLock g_rwLock;
	MAP_THREAD_WINDOWS_HOOK map_thread_windows_hook;

	UINT nHtmlMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));

	typedef LRESULT (__stdcall *PFN_Porc)(HWND, UINT, WPARAM, LPARAM);

	LRESULT WINAPI NewWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = 0;

		if ( Msg == nHtmlMsg || Msg == WM_ERASEBKGND )
		{
			result = 0;
		}
		else if ((Msg == WM_GETOBJECT) && (OBJID_WINDOW == (DWORD)lParam))
		{
			result = 0;
		}
		else
		{
			PFN_Porc pOldIEPorc = (PFN_Porc)GetPropA(hWnd, "OldIEPorc");
			if ( pOldIEPorc )
			{
				result = CallWindowProcW(pOldIEPorc, hWnd, Msg, wParam, lParam);
			}
			else
			{
				result = DefWindowProcW(hWnd, Msg, wParam, lParam);
			}
		}
		return result;
	}

	LRESULT WINAPI HookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		PCWPSTRUCT pCWPstruct = (PCWPSTRUCT)lParam;
		CStringA ClassName;
		if ( pCWPstruct && ( pCWPstruct->message == WM_CREATE) )
		{
			HWND hWnd = pCWPstruct->hwnd;
			GetClassNameA(hWnd, ClassName.GetBuffer(MAX_CLASS_NAME), MAX_CLASS_NAME);
			ClassName.ReleaseBuffer();
			if ( ClassName == "Internet Explorer_Server")
			{
				ClassName.Empty();
				HWND hParent = GetParent(hWnd);
				GetClassNameA(hParent, ClassName.GetBuffer(MAX_CLASS_NAME), MAX_CLASS_NAME);
				ClassName.ReleaseBuffer();
				if ( ClassName != "Internet Explorer_Server" )
				{
					LONG res = SetWindowLongW(hWnd, GWL_WNDPROC/*-4*/, (LONG)NewWndProc);
					if ( res )
					{
						SetPropA(hWnd, "OldIEPorc", (HANDLE)res);
					}
				}
			}
		}
		if (!ClassName.IsEmpty()) ClassName.Empty();

		HHOOK hWindowsHook = NULL;
		DWORD dwCurHookProcThreadID = GetCurrentThreadId();

		g_rwLock.rlock();
		MAP_THREAD_WINDOWS_HOOK_ITERATOR it = map_thread_windows_hook.find(dwCurHookProcThreadID);
		if (it != map_thread_windows_hook.end())
		{
			hWindowsHook = it->second;
		}
		g_rwLock.unlock();

		LRESULT lRes;
		if (hWindowsHook)
		{
			lRes = CallNextHookEx(hWindowsHook, nCode, wParam, lParam);
		}
		else
		{
			lRes = 0;
		}

		return lRes;
	}
}

void BeginShieldHtmlMsg()
{
	DWORD dwCurThread = GetCurrentThreadId();
	HHOOK hWindowsHook = ::SetWindowsHookExA(WH_CALLWNDPROC,HookProc,NULL,dwCurThread);
	if (hWindowsHook)
	{
		g_rwLock.wlock();
		MAP_THREAD_WINDOWS_HOOK_ITERATOR it = map_thread_windows_hook.find(dwCurThread);
		if (it == map_thread_windows_hook.end())
		{
			map_thread_windows_hook.insert(std::make_pair(dwCurThread,hWindowsHook));
		}
		g_rwLock.unlock();
	}
}

void EndShieldHtmlMsg()
{
	DWORD dwCurThread = GetCurrentThreadId();
	g_rwLock.wlock();
	MAP_THREAD_WINDOWS_HOOK_ITERATOR it = map_thread_windows_hook.find(dwCurThread);
	if (it != map_thread_windows_hook.end())
	{
		HHOOK hWindowsHook = it->second;
		if (hWindowsHook)
		{
			::UnhookWindowsHookEx(hWindowsHook);
		}
		map_thread_windows_hook.erase(it);
	}
	g_rwLock.unlock();
}

