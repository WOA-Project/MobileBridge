#include "Audio Syncing Controller.h"
#include "Topology Telephony Controller.h"
#include <endpointvolume.h>
#include <iostream>

HRESULT hr;
IMMDeviceEnumerator* deviceEnumerator = NULL;

TelephonyController Telephony;

float GetSystemVolume() {
	HRESULT hr;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDevice* defaultDevice = NULL;
	IAudioEndpointVolume* endpointVolume = NULL;
	float currentVolume = 1;

	hr = CoInitialize(NULL);

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);

	if (deviceEnumerator != NULL)
	{
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		deviceEnumerator->Release();
		deviceEnumerator = NULL;
	}

	if (defaultDevice != NULL)
	{
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
		defaultDevice->Release();
		defaultDevice = NULL;
	}

	if (endpointVolume != NULL)
	{
		hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
		endpointVolume->Release();
	}

	CoUninitialize();

	return currentVolume;
}

BOOL GetIsMuted() {
	HRESULT hr;

	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDevice* defaultDevice = NULL;
	IAudioEndpointVolume* endpointVolume = NULL;
	BOOL currentMute = FALSE;

	hr = CoInitialize(NULL);

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);

	if (deviceEnumerator != NULL)
	{
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		deviceEnumerator->Release();
		deviceEnumerator = NULL;
	}

	if (defaultDevice != NULL)
	{
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
		defaultDevice->Release();
		defaultDevice = NULL;
	}

	if (endpointVolume != NULL)
	{
		endpointVolume->GetMute(&currentMute);
		endpointVolume->Release();
	}

	CoUninitialize();

	return currentMute;
}

class CVolumeNotification : public IAudioEndpointVolumeCallback
{
	LONG m_RefCount;
	~CVolumeNotification(void) {};
public:
	CVolumeNotification(void) : m_RefCount(1)
	{
	}
	STDMETHODIMP_(ULONG)AddRef() { return InterlockedIncrement(&m_RefCount); }
	STDMETHODIMP_(ULONG)Release()
	{
		LONG ref = InterlockedDecrement(&m_RefCount);
		if (ref == 0)
			delete this;
		return ref;
	}
	STDMETHODIMP QueryInterface(REFIID IID, void** ReturnValue)
	{
		if (IID == IID_IUnknown || IID == __uuidof(IAudioEndpointVolumeCallback))
		{
			*ReturnValue = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		*ReturnValue = NULL;
		return E_NOINTERFACE;
	}

	STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA NotificationData)
	{
		float DesiredVolume = NotificationData->fMasterVolume;
		BOOL DesiredMute = NotificationData->bMuted;

		if (DesiredMute)
		{
			Telephony.SetTelephonyVolume(0);
		}
		else
		{
			Telephony.SetTelephonyVolume(DesiredVolume);
		}

		return S_OK;
	}
};

class CVolumeOtherNotification : public IAudioEndpointVolumeCallback
{
	LONG m_RefCount;
	~CVolumeOtherNotification(void) {};
public:
	CVolumeOtherNotification(void) : m_RefCount(1)
	{
	}
	STDMETHODIMP_(ULONG)AddRef() { return InterlockedIncrement(&m_RefCount); }
	STDMETHODIMP_(ULONG)Release()
	{
		LONG ref = InterlockedDecrement(&m_RefCount);
		if (ref == 0)
			delete this;
		return ref;
	}
	STDMETHODIMP QueryInterface(REFIID IID, void** ReturnValue)
	{
		if (IID == IID_IUnknown || IID == __uuidof(IAudioEndpointVolumeCallback))
		{
			*ReturnValue = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		*ReturnValue = NULL;
		return E_NOINTERFACE;
	}

	STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA NotificationData)
	{
		float CurrentVolume = NotificationData->fMasterVolume;
		BOOL CurrentMute = NotificationData->bMuted;

		float DesiredVolume = GetSystemVolume();
		BOOL DesiredMute = GetIsMuted();

		if (CurrentVolume != DesiredVolume ||
			CurrentMute != DesiredMute)
		{
			if (DesiredMute)
			{
				Telephony.SetTelephonyVolume(0);
			}
			else
			{
				Telephony.SetTelephonyVolume(DesiredVolume);
			}
		}

		return S_OK;
	}
};

class CEndpointNotification : public IMMNotificationClient
{
	LONG m_RefCount;
	IAudioEndpointVolume* endpointVolume;
	CVolumeNotification* volumeNotification;

	~CEndpointNotification(void) {};
public:
	CEndpointNotification(void) : m_RefCount(1)
	{
		if (endpointVolume != NULL)
		{
			if (volumeNotification != NULL)
			{
				endpointVolume->UnregisterControlChangeNotify(volumeNotification);
				volumeNotification->Release();
				volumeNotification = NULL;
			}
		}

		IMMDevice* defaultDevice = NULL;

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
		volumeNotification = new CVolumeNotification();
		hr = endpointVolume->RegisterControlChangeNotify(volumeNotification);
		defaultDevice->Release();
		defaultDevice = NULL;
	}
	STDMETHODIMP_(ULONG)AddRef() { return InterlockedIncrement(&m_RefCount); }
	STDMETHODIMP_(ULONG)Release()
	{
		if (endpointVolume != NULL)
		{
			if (volumeNotification != NULL)
			{
				endpointVolume->UnregisterControlChangeNotify(volumeNotification);
				volumeNotification->Release();
			}
			endpointVolume->Release();
		}

		LONG ref = InterlockedDecrement(&m_RefCount);
		if (ref == 0)
			delete this;
		return ref;
	}
	STDMETHODIMP QueryInterface(REFIID IID, void** ReturnValue)
	{
		if (IID == IID_IUnknown || IID == __uuidof(IAudioEndpointVolumeCallback))
		{
			*ReturnValue = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		*ReturnValue = NULL;
		return E_NOINTERFACE;
	}

	STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
	{
		if (endpointVolume != NULL)
		{
			if (volumeNotification != NULL)
			{
				endpointVolume->UnregisterControlChangeNotify(volumeNotification);
				volumeNotification->Release();
				volumeNotification = NULL;
			}
		}

		IMMDevice* defaultDevice = NULL;

		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
		volumeNotification = new CVolumeNotification();
		hr = endpointVolume->RegisterControlChangeNotify(volumeNotification);
		defaultDevice->Release();
		defaultDevice = NULL;

		return S_OK;
	}

	STDMETHODIMP OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) { return S_OK; }
	STDMETHODIMP OnDeviceAdded(LPCWSTR pwstrDeviceId) { return S_OK; }
	STDMETHODIMP OnDeviceRemoved(LPCWSTR pwstrDeviceId) { return S_OK; }
	STDMETHODIMP OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) { return S_OK; }
	STDMETHODIMP OnDeviceQueryRemove() { return S_OK; }
	STDMETHODIMP OnDeviceQueryRemoveFailed() { return S_OK; }
	STDMETHODIMP OnDeviceRemovePending() { return S_OK; }
};

CEndpointNotification* endpointNotification = NULL;

HRESULT AudioSyncingController::Initialize()
{
	float currentVolume = 0;
	BOOL currentMute = NULL;

	CComPtr<IAudioEndpointVolume> endpointVolume = NULL;
	CComPtr<IMMDevice>            defaultDevice  = NULL;
	CVolumeOtherNotification*     volumeNotification;

	std::cout << "AudioSyncingController::Initialize -> Telephony.Initialize" << std::endl;
	const auto TopologyTelInitialiseResult = Telephony.Initialize();

	std::cout << "AudioSyncingController::Initialize -> GetSystemVolume" << std::endl;
	currentVolume = GetSystemVolume();
	std::cout << "AudioSyncingController::Initialize -> GetIsMuted" << std::endl;
	currentMute = GetIsMuted();

	if (currentMute)
	{
		std::cout << "AudioSyncingController::Initialize -> SetTelephonyVolume(0)" << std::endl;
		Telephony.SetTelephonyVolume(0);
	}
	else
	{
		std::cout << "AudioSyncingController::Initialize -> SetTelephonyVolume" << std::endl;
		Telephony.SetTelephonyVolume(currentVolume);
	}

	std::cout << "AudioSyncingController::Initialize -> CoInitialize" << std::endl;
	hr = CoInitialize(NULL);

	std::cout << "AudioSyncingController::Initialize -> CoCreateInstance" << std::endl;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);

	if (deviceEnumerator != NULL)
	{
		std::cout << "AudioSyncingController::Initialize -> CEndpointNotification" << std::endl;
		endpointNotification = new CEndpointNotification();
		std::cout << "AudioSyncingController::Initialize -> RegisterEndpointNotificationCallback" << std::endl;
		hr = deviceEnumerator->RegisterEndpointNotificationCallback(endpointNotification);
	}

	//
	// Register events in case both call and fm radio endpoint change volume without us changing it.
	//
	std::cout << "AudioSyncingController::Initialize -> CVolumeOtherNotification" << std::endl;
	volumeNotification = new CVolumeOtherNotification();

	std::cout << "AudioSyncingController::Initialize -> GetMMDevice" << std::endl;
	defaultDevice = Telephony.GetMMDevice();

	if (defaultDevice != NULL)
	{
		std::cout << "AudioSyncingController::Initialize -> Activate" << std::endl;
		hr = defaultDevice->Activate(
			__uuidof(IAudioEndpointVolume),
			CLSCTX_ALL,
			NULL,
			(VOID**)&endpointVolume);

		if (SUCCEEDED(hr))
		{
			std::cout << "AudioSyncingController::Initialize -> RegisterControlChangeNotify" << std::endl;
			hr = endpointVolume->RegisterControlChangeNotify(volumeNotification);
		}

		defaultDevice = NULL;
	}

	return ERROR_SUCCESS;
}

HRESULT AudioSyncingController::DeInitialize()
{
	if (deviceEnumerator != NULL)
	{
		hr = deviceEnumerator->UnregisterEndpointNotificationCallback(endpointNotification);
		endpointNotification->Release();
		deviceEnumerator->Release();
		deviceEnumerator = NULL;
	}

	CoUninitialize();
	return ERROR_SUCCESS;
}