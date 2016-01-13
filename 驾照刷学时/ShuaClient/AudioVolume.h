#pragma once

#include <mmdeviceapi.h>
#include <Endpointvolume.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <vector>


// ≥ı ºªØæ≤“Ù
IMMDeviceEnumerator* TxInitMuted();

//  Õ∑≈
bool TxUnMuted();

// …Ë÷√æ≤“Ù
bool TxSetMuted(bool bMuted);

 
 class CCoreAudioVolume
 {
 public:
 	static HRESULT EnableSound(BOOL bEnable);
 
 	static HRESULT Initlialize(BOOL bEnableSound);
 
 	static HRESULT Uninitialize();
	
 private:
 	static bool GetDeviceDsc(IMMDevice *pDevice,wchar_t* DeviceDsc);
 
 	static bool VerifyDev(IMMDevice *pDevice,EDataFlow dataFlow);
 
 	static int GetDevicePlayVol(void);
 
 	static bool SetMute(BOOL bMute);
 
 private:
 	static BOOL m_bEnableSound;
 	static IMMDeviceEnumerator*		m_pEnumerator;  
 	static IMMDeviceCollection*		m_pCollection;  
 	//	static IAudioEndpointVolume*	pVolumeAPI=NULL;  
 	static UINT						m_nDevCount;
	static IAudioSessionControl*    m_pAudioSection;
 	static std::vector<IMMDevice*>	m_vArrayDevice;
 };
//////////////////////////////////////////////////////////////////////////
class CMutedSessionEvents : public IAudioSessionEvents
{
public:
	CMutedSessionEvents(IAudioSessionManager*    pAudioSessionManager );
	~CMutedSessionEvents(){}

	// IUnknown methods -- AddRef, Release, and QueryInterface

	ULONG STDMETHODCALLTYPE AddRef(){return InterlockedIncrement(&_cRef);}

	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID  riid,
		VOID  **ppvInterface);

	// Notification methods for audio session events

	HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(
		LPCWSTR NewDisplayName,
		LPCGUID EventContext)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnIconPathChanged(
		LPCWSTR NewIconPath,
		LPCGUID EventContext)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext);

	HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(
		DWORD ChannelCount,
		float NewChannelVolumeArray[],
		DWORD ChangedChannel,
		LPCGUID EventContext)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(
		LPCGUID NewGroupingParam,
		LPCGUID EventContext)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnStateChanged(
		AudioSessionState NewState)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		AudioSessionDisconnectReason DisconnectReason)
	{
		return S_OK;
	}

	bool TxSetMuted(bool bMuted);

private:
	LONG _cRef;

private:
	IAudioSessionManager*        m_AudioSessionManager;
};
//////////////////////////////////////////////////////////////////////////
class CAudioVolume
{
public:
	CAudioVolume(void);
	~CAudioVolume(void);
};
