#include "stdafx.h"
#include "AudioVolume.h"

CAudioVolume::CAudioVolume(void)
{
}

CAudioVolume::~CAudioVolume(void)
{
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool g_bMuted=true;
//static IMMDeviceEnumerator*        m_pEnumerator                = NULL;
//static IMMDevice*                  m_pDeviceOut                    = NULL;
//static IAudioSessionManager*        m_AudioSessionManager = NULL;
IAudioSessionControl*        CCoreAudioVolume::m_pAudioSection    = NULL;
//static CMutedSessionEvents *        m_pMutedSessionEvents    = NULL;
//
//BOOL IsWindows7()
//{
//	OSVERSIONINFO osinfo;
//	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//
//	if (!GetVersionEx(&osinfo))
//		return FALSE;
//
//	DWORD dwPlatformId   = osinfo.dwPlatformId;
//	DWORD dwMinorVersion = osinfo.dwMinorVersion;
//	DWORD dwMajorVersion = osinfo.dwMajorVersion;
//	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;    // Win 95 needs this
//
//	if (dwPlatformId == 2)
//	{
//		if ((dwMajorVersion == 6) && (dwMinorVersion == 1))
//		{
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//IMMDeviceEnumerator*  TxInitMuted()
//{
//	CoInitializeEx( NULL , COINIT_MULTITHREADED );    
//
//	HRESULT hr;  
//
//	//ʵ���� MMDeviceEnumerator ö����   
//	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&m_pEnumerator);    
//	if(FAILED(hr))  
//	{  
//		return NULL;  
//	}  
//
//	// Ĭ���豸��IMMDevice�ӿ�
//	hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_pDeviceOut);
//	if (FAILED(hr)) return false;
//
//	// �õ���Ƶ�Ự�ӿ�IAudioSessionManager
//	hr = m_pDeviceOut->Activate(__uuidof(m_AudioSessionManager), CLSCTX_ALL, NULL, (void**)&m_AudioSessionManager);
//	if (FAILED(hr)) return false;
//
//	// ���ûỰ(Session)����
//	hr = m_AudioSessionManager->GetAudioSessionControl(NULL,0,&m_AudioSessionControl);
//	if (FAILED(hr)) return false;
//
//	// ���������ı����֪ͨ
//	m_pMutedSessionEvents = new CMutedSessionEvents(m_AudioSessionManager);
//	hr = m_AudioSessionControl->RegisterAudioSessionNotification(m_pMutedSessionEvents);
//	if (FAILED(hr)) return false;
//
//	return m_pEnumerator;
//}
//
//bool TxUnMuted()
//{
//	// ---- ��ע�������ı����֪ͨ
//	if (m_AudioSessionControl!=NULL&&m_pMutedSessionEvents!=NULL)
//	{
//		m_AudioSessionControl->UnregisterAudioSessionNotification(m_pMutedSessionEvents);
//	}
//
//	// ---- �ͷ��ڴ�ռ�
//	if (m_AudioSessionControl != NULL)
//	{
//		m_AudioSessionControl->Release(); 
//		m_AudioSessionControl = NULL;
//	}
//	if (m_pMutedSessionEvents != NULL)
//	{
//		m_pMutedSessionEvents->Release(); 
//		m_pMutedSessionEvents = NULL;
//	}
//	if (m_pDeviceOut != NULL)
//	{
//		m_pDeviceOut->Release();
//		m_pDeviceOut = NULL;
//	}
//	if(m_AudioSessionManager != NULL)
//	{
//		m_AudioSessionManager->Release();
//		m_AudioSessionManager = NULL;
//	}
//	if (m_pEnumerator != NULL)
//	{
//		m_pEnumerator->Release();
//		m_pEnumerator = NULL;
//	}
//
//	CoUninitialize(); 
//
//	return true;
//}
//
//bool TxSetMuted(bool bMuted)
//{
//	HRESULT hr = S_FALSE;
//
//	// ����������������ƽӿ�
//	ISimpleAudioVolume   *pAudioVolume = NULL;
//	hr = m_AudioSessionManager->GetSimpleAudioVolume(NULL,0,&pAudioVolume);
//	if (FAILED(hr)) return false;
//
//	if(SUCCEEDED(hr))  
//	{  
//		pAudioVolume->SetMute(bMuted, &GUID_NULL);  
//
//		// ����ȫ�־���״̬
//		g_bMuted = bMuted;
//	}  
//
//	if (pAudioVolume != NULL)
//	{
//		pAudioVolume->Release();
//		pAudioVolume = NULL;
//	}
//	return true;
//}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(punk)  \
	if ((punk) != NULL)  \
{ (punk)->Release(); (punk) = NULL; }

BOOL					CCoreAudioVolume::m_bEnableSound = FALSE;
IMMDeviceEnumerator*	CCoreAudioVolume::m_pEnumerator = NULL;  
IMMDeviceCollection*	CCoreAudioVolume::m_pCollection = NULL;  
UINT					CCoreAudioVolume::m_nDevCount = 0;
std::vector<IMMDevice*>	CCoreAudioVolume::m_vArrayDevice;
//static IAudioSessionControl*        CCoreAudioVolume::m_AudioSessionControl    = NULL;

//�õ��豸Ӳ��ID (�豸���������Կ�����Ӳ��ID)
bool CCoreAudioVolume::GetDeviceDsc(IMMDevice *pDevice,wchar_t* DeviceDsc)  
{
	HRESULT hr;
	IPropertyStore *pStore;
	hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);
	if (SUCCEEDED(hr))
	{
		PROPERTYKEY Drvidkey ={0xb3f8fa53, 0x0004, 0x438e, 0x90, 0x03, 0x51, 0xa4, 0x6e, 0x13, 0x9b, 0xfc, 2};
		PROPVARIANT pDrvidkey;
		PropVariantInit(&pDrvidkey);
		hr = pStore->GetValue(Drvidkey , &pDrvidkey);  
		if (SUCCEEDED(hr))
		{
			wcscpy(DeviceDsc,pDrvidkey.pwszVal);
			PropVariantClear(&pDrvidkey);
			pStore->Release();
			return true;
		}
		pStore->Release();
	}
	return false;
}

// ��֤�豸�Ƿ�ָ���豸
bool CCoreAudioVolume::VerifyDev(IMMDevice *pDevice,EDataFlow dataFlow)
{
	wchar_t DeviceDsc[255]; 
	if (GetDeviceDsc(pDevice,DeviceDsc)) 
	{
		// ����ʡ���жϾ����豸�� ƥ��Ӳ������ HDAUDIO\FUNC_01&VEN_10EC&DEV_0888&SUBSYS_14627514&REV_1000
		return true;
	}
	return false;
}

// ��ȡ�豸����
int CCoreAudioVolume::GetDevicePlayVol(void)
{   
	IMMDeviceEnumerator* pEnumerator;  
	IMMDeviceCollection* pCollection = NULL;  
	IMMDevice *pDevice = NULL;  
	IAudioEndpointVolume *pVolumeAPI=NULL;    
	UINT deviceCount = 0;  
	HRESULT hr;
	float fVolume = -1;  

	CoInitializeEx( NULL , COINIT_MULTITHREADED );  
	//ʵ���� MMDeviceEnumerator ö����
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);  
	if (hr != S_OK)    
	{    
		goto FreeEnumerator;
	}    
	// ö�� �豸���豸���� eRander�������豸��DEVICE_STATE_ACTIVE Ϊ��ǰ�Ѽ�����豸�����ú������ӵ�������״̬����
	hr = pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATE_ACTIVE , &pCollection );  
	if (hr != S_OK)    
	{       
		goto FreeCollection; 
	}    
	// �豸�����������
	hr = pCollection->GetCount(&deviceCount);    
	if (hr != S_OK)    
	{    
		goto FreeCollection;
	}    

	for (UINT dev=0; dev<deviceCount; dev++)    
	{    
		pDevice = NULL;    
		hr = pCollection->Item(dev,&pDevice);    
		if (hr == S_OK)    
		{
			if (VerifyDev(pDevice,eRender)) 
			{    // �� pDevice �� Activate ������ʼһ�� IAudioEndpointVolume �ӿ�
				hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void **)(&pVolumeAPI));    
				// ʹ�� IAudioEndpintVolume �ķ�����ȡ�������������������þ����� 
				hr = pVolumeAPI->GetMasterVolumeLevelScalar(&fVolume);
				break;
			}
		}    
	}  

FreeCollection:
	SAFE_RELEASE(pCollection);
FreeEnumerator:
	SAFE_RELEASE(pEnumerator);
	CoUninitialize();
	if (fVolume > 0) 
		return fVolume*100; 
	else
		return fVolume;

	return 0;
}

//����Ӧ�ó�����
bool CCoreAudioVolume::SetMute(BOOL bMute)
{
	//IAudioSessionControl *pAudioSection = NULL;
	ISimpleAudioVolume	 *pAudioVolume = NULL;
	IAudioSessionManager *pManager = NULL;
	IMMDevice			 *pDevice = NULL;			
	HRESULT hr;

	std::vector<IMMDevice*>::iterator iter = m_vArrayDevice.begin();
	for(; iter != m_vArrayDevice.end(); iter++)
	{
		pDevice = *iter;
		ATLASSERT(pDevice != NULL);

		hr = pDevice->Activate(__uuidof(IAudioSessionManager),
			CLSCTX_INPROC_SERVER, NULL,
			(void**)&pManager);

		if(FAILED(hr)) continue;

		hr = pManager->GetAudioSessionControl(NULL, 0, &m_pAudioSection);
		CMutedSessionEvents* pMutedse=new CMutedSessionEvents(pManager);
		hr = m_pAudioSection->RegisterAudioSessionNotification(pMutedse);
		hr = pManager->GetSimpleAudioVolume(NULL, 0, &pAudioVolume);
		
		if(SUCCEEDED(hr))
		{
			pAudioVolume->SetMute(bMute, &GUID_NULL);
			//pAudioVolume->SetMasterVolume()
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}

	SAFE_RELEASE(pManager);
	SAFE_RELEASE(pAudioVolume);

	return true;
}

HRESULT CCoreAudioVolume::EnableSound(BOOL bEnable)
{
	m_bEnableSound = bEnable;

	bool bResult = CCoreAudioVolume::SetMute(!bEnable);

	return bResult ? S_OK : E_FAIL;
}

HRESULT CCoreAudioVolume::Initlialize(BOOL bEnableSound)
{
	m_bEnableSound = bEnableSound;

	CoInitializeEx( NULL , COINIT_MULTITHREADED );  

	HRESULT hr;
	//ʵ���� MMDeviceEnumerator ö����
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&m_pEnumerator);  
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	// ö�� �豸���豸���� eRander�������豸��DEVICE_STATE_ACTIVE Ϊ��ǰ�Ѽ�����豸�����ú������ӵ�������״̬����
	hr = m_pEnumerator->EnumAudioEndpoints( eRender , DEVICE_STATE_ACTIVE , &m_pCollection );  
	if (hr != S_OK)    
	{       
		return E_FAIL;
	}  

	// �豸�����������
	hr = m_pCollection->GetCount(&m_nDevCount);    
	if (hr != S_OK)    
	{    
		return E_FAIL;
	}    

	IMMDevice* pDevice = NULL;

	for (UINT dev=0; dev<m_nDevCount; dev++)    
	{    
		pDevice = NULL;    
		hr = m_pCollection->Item(dev,&pDevice);    
		if (hr == S_OK)    
		{
			if (VerifyDev(pDevice,eRender)) 
			{   
				m_vArrayDevice.push_back(pDevice);
			}
		}    
	} 

	return S_OK;
}

HRESULT CCoreAudioVolume::Uninitialize()
{
	std::vector<IMMDevice*>::iterator iter = m_vArrayDevice.begin();
	for(; iter != m_vArrayDevice.end(); iter++)
	{
		SAFE_RELEASE(*iter);
	}
	SAFE_RELEASE(m_pCollection);
	SAFE_RELEASE(m_pEnumerator);

	CoUninitialize();
	m_vArrayDevice.clear();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//ʵ�ֽӿ�
//////////////////////////////////////////////////////////////////////////
//extern volatile bool g_bMuted;
CMutedSessionEvents::CMutedSessionEvents(IAudioSessionManager*    pAudioSessionManager)
{
	m_AudioSessionManager = pAudioSessionManager;
	_cRef = 1;
}

ULONG STDMETHODCALLTYPE CMutedSessionEvents::Release()
{
	ULONG ulRef = InterlockedDecrement(&_cRef);
	if (0 == ulRef)
	{
		delete this;
	}
	return ulRef;
}

HRESULT STDMETHODCALLTYPE CMutedSessionEvents::QueryInterface(
	REFIID  riid,
	VOID  **ppvInterface)
{
	if (IID_IUnknown == riid)
	{
		AddRef();
		*ppvInterface = (IUnknown*)this;
	}
	else if (__uuidof(IAudioSessionEvents) == riid)
	{
		AddRef();
		*ppvInterface = (IAudioSessionEvents*)this;
	}
	else
	{
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMutedSessionEvents::OnSimpleVolumeChanged(
	float NewVolume,
	BOOL NewMute,
	LPCGUID EventContext)
{
	// ��ֹ��������Ծ����۸�
	if ((BOOL)g_bMuted != NewMute)
	{
		TxSetMuted(g_bMuted);
	}

	return S_OK;
}

bool CMutedSessionEvents::TxSetMuted(bool bMuted)
{
	HRESULT hr = S_FALSE;

	// ����������������ƽӿ�
	ISimpleAudioVolume   *pAudioVolume = NULL;
	hr = m_AudioSessionManager->GetSimpleAudioVolume(NULL,0,&pAudioVolume);
	if (FAILED(hr)) return false;

	if(SUCCEEDED(hr))  
	{  
		pAudioVolume->SetMute(bMuted, &GUID_NULL);  

		// ����ȫ�־���״̬
		g_bMuted = bMuted;
	}  

	if (pAudioVolume != NULL)
	{
		pAudioVolume->Release();
		pAudioVolume = NULL;
	}
	return true;
}