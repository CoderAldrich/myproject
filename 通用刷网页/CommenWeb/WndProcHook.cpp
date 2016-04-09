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
		* 假设当前有多个读锁请求正在等待写锁释放,那么当写锁被释放时,所有这些读锁都应该有机会获得执行.
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
				* 等待事件返回,重新竞争锁.
				*/
				--_rwaitingCount;
			}

			if(_st == RWLOCK_IDLE)
			{
				/*
				* 空闲状态,直接得到控制权
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
					* 有写锁正在等待,则一起等待,以使写锁能获得竞争机会.
					*/
					++_rwaitingCount;
					ResetEvent(_ev);
					//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
					LeaveCriticalSection(&_stLock);

					/*
					* 虽然 LeaveCriticalSection() 和 WaitForSingleObject() 之间有一个时间窗口,
					* 但是由于windows平台的事件信号是不会丢失的,所以没有问题.
					*/
					WaitForSingleObject(_ev, INFINITE);

					/*
					* 等待返回,继续尝试加锁.
					*/
					isWaitReturn = true;
				}
				else
				{	
					/*
					* 得到读锁,计数+1
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
				* 等待写锁释放
				*/
				++_rwaitingCount;
				ResetEvent(_ev);
				//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
				LeaveCriticalSection(&_stLock);
				WaitForSingleObject(_ev, INFINITE);

				/*
				* 等待返回,继续尝试加锁.
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
			/* 读锁解锁 */
			--_rlockCount;

			if( 0 == _rlockCount)
			{
				_st = RWLOCK_IDLE;

				/* 释放 */
				if( _wwaitingCount > 0 || _rwaitingCount > 0 )
				{
					/* 
					* 此时有锁请求正在等待,激活所有等待的线程.(手动事件).
					* 使这些请求重新竞争锁.
					*/
					SetEvent(_ev);
				}
				else
				{
					/* 空闲 */
				}
			}
			else
			{
				/* 还有读锁 */
			}
		}
		else
		{
			_st = RWLOCK_IDLE;

			/* 写锁解锁 */
			if( _wwaitingCount > 0 || _rwaitingCount > 0 )
			{
				/* 
				* 如果在占有互斥量_stLock的情况下,触发事件,那么可能会使一些锁请求不能得到竞争机会.
				* 假设调用unlock时,另一个线程正好调用rlock或者wlock.如果不释放互斥量,只有之前已经等待的锁请求有机会获得锁控制权.
				*/
				SetEvent(_ev);
			}
			else
			{
				/* 空闲 */
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

