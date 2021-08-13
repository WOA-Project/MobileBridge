#include "Brightness Sensor Controller.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Sensors.h>
#include <winrt/Windows.Internal.Graphics.Display.DisplayEnhancementManagement.h>
#include <Ntddvdeo.h>
#include <thread>
#include <powrprof.h>

#include <WinIoCtl.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Sensors;
using namespace winrt::Windows::Internal::Graphics::Display::DisplayEnhancementManagement;

#define WINDOWS_AUTO_BRIGHTNESS_KEY_PATH L"SYSTEM\\CurrentControlSet\\Services\\DisplayEnhancementService\\State\\CMO00110_09_07D9_98"

DisplayEnhancementManagement dispEnhMan = NULL;
LightSensor sensor = LightSensor::GetDefault();

//
// The system registry key for auto brightness
//
HKEY autoBrightnessKey = NULL;

//
// Are we already registered with the sensor?
//
BOOL AlreadySetup = FALSE;

//
// Is the event subscribed
//
BOOL Subscribed = FALSE;

//
// The event token for the orientation sensor on orientation changed event
//
event_token eventToken;

//
// The handle the power notify event registration
//
HPOWERNOTIFY m_systemSuspendHandle = NULL;
HPOWERNOTIFY m_hScreenStateNotify = NULL;

//
// Subject: Change screen brightness
//
// Parameters:
//
//             brightness:
//
// Returns: void
//
void UpdateDisplayBrightness(int brightness)
{
	HKEY displayKey = nullptr;
	DWORD IsAutobrightnessOn = 0;
	DWORD type = 0;
	DWORD size = sizeof(IsAutobrightnessOn);
	DWORD ret = NULL;

	DISPLAY_BRIGHTNESS _displayBrightness;
	DWORD nOutBufferSize = sizeof(_displayBrightness);
	HANDLE h = NULL;

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINDOWS_AUTO_BRIGHTNESS_KEY_PATH, 0, KEY_READ, &displayKey);

	RegQueryValueEx(displayKey, L"IsAutobrightnessOn", NULL, &type, (LPBYTE)&IsAutobrightnessOn, &size);

	RegCloseKey(displayKey);

	//if (!dispEnhMan.IsAutobrightnessOn())
	if (!IsAutobrightnessOn)
	{
		return;
	}

	_displayBrightness.ucDisplayPolicy = 0;
	_displayBrightness.ucACBrightness = brightness;
	_displayBrightness.ucDCBrightness = brightness;

	h = CreateFile(L"\\\\.\\LCD",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}

	if (!DeviceIoControl(h, IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS, (DISPLAY_BRIGHTNESS*)&_displayBrightness, nOutBufferSize, NULL, 0, &ret, NULL))
	{
		return;
	}
}

int previousstate = -20;

bool IsRadicallyDifferent(int targetBrightness)
{
	if (abs(targetBrightness - previousstate) >= 20)
	{
		previousstate = targetBrightness;
		UpdateDisplayBrightness(targetBrightness);
		return TRUE;
	}
	return FALSE;
}

void InitializeDisplayEnhancementManagement()
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	size_t outSize;

	EnumDisplayDevices(0, 0, &dd, 0);
	EnumDisplayDevices(dd.DeviceName, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
	//dispEnhMan = DisplayEnhancementManagement::FromIdAsync(dd.DeviceID).get(); // crashes in service (?)
}

VOID OnLightSensorReadingChanged(IInspectable const& /*sender*/, LightSensorReadingChangedEventArgs const& args)
{
	LightSensorReading readings = args.Reading();
	float lux = readings.IlluminanceInLux();

	int targetBrightness = floor(lux / 10);

	if (targetBrightness > 100)
		targetBrightness = 100;

	printf("Brightness: %d\n", targetBrightness);

	IsRadicallyDifferent(targetBrightness);
}

VOID BrightnessSensorController::OnPowerEvent(
	_In_ GUID SettingGuid,
	_In_ PVOID Value,
	_In_ ULONG ValueLength,
	_Inout_opt_ PVOID Context
)
{
	UNREFERENCED_PARAMETER(Context);

	if (IsEqualGUID(GUID_CONSOLE_DISPLAY_STATE, SettingGuid))
	{
		if (ValueLength != sizeof(DWORD))
		{
			return;
		}

		DWORD DisplayState = *(DWORD*)Value;

		switch (DisplayState)
		{
		case 0:
			// Display Off

			//
			// Unsubscribe to sensor events
			//
			if (Subscribed)
			{
				sensor.ReadingChanged(eventToken);
				Subscribed = FALSE;
			}
			break;
		case 1:
			// Display On

			//
			// Subscribe to sensor events
			//
			if (!Subscribed)
			{
				eventToken = sensor.ReadingChanged(OnLightSensorReadingChanged);
				Subscribed = TRUE;
			}
			break;
		case 2:
			// Display Dimmed

			break;
		default:
			// Unknown
			break;
		}
	}
}

VOID OnSystemSuspendStatusChanged(ULONG PowerEvent)
{
	if (PowerEvent == PBT_APMSUSPEND)
	{
		// Entering

		//
		// Unsubscribe to sensor events
		//
		if (Subscribed)
		{
			sensor.ReadingChanged(eventToken);
			Subscribed = FALSE;
		}
	}
	else if (PowerEvent == PBT_APMRESUMEAUTOMATIC || PowerEvent == PBT_APMRESUMESUSPEND)
	{
		// AutoResume
		// ManualResume

		//
		// Subscribe to sensor events
		//
		if (!Subscribed)
		{
			eventToken = sensor.ReadingChanged(OnLightSensorReadingChanged);
			Subscribed = TRUE;
		}
	}
}

// Using PVOID as per the actual typedef
ULONG CALLBACK SuspendResumeCallback(PVOID context, ULONG powerEvent, PVOID setting)
{
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(setting);
	OnSystemSuspendStatusChanged(powerEvent);
	return ERROR_SUCCESS;
}

VOID UnregisterEverything()
{
	if (m_systemSuspendHandle != NULL)
	{
		PowerUnregisterSuspendResumeNotification(m_systemSuspendHandle);
		m_systemSuspendHandle = NULL;
	}

	if (m_hScreenStateNotify != NULL)
	{
		UnregisterPowerSettingNotification(m_hScreenStateNotify);
		m_hScreenStateNotify = NULL;
	}

	//
	// Unsubscribe to sensor events
	//
	if (Subscribed)
	{
		sensor.ReadingChanged(eventToken);
		Subscribed = FALSE;
	}

	AlreadySetup = FALSE;
}

VOID RegisterEverything(SERVICE_STATUS_HANDLE g_StatusHandle)
{
	DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS powerParams{};
	powerParams.Callback = SuspendResumeCallback;

	PowerRegisterSuspendResumeNotification(
		DEVICE_NOTIFY_CALLBACK,
		&powerParams,
		&m_systemSuspendHandle
	);

	if (g_StatusHandle != NULL)
	{
		m_hScreenStateNotify = RegisterPowerSettingNotification(
			g_StatusHandle,
			&GUID_CONSOLE_DISPLAY_STATE,
			DEVICE_NOTIFY_SERVICE_HANDLE
		);
	}

	//
	// Subscribe to sensor events
	//
	if (!Subscribed)
	{
		eventToken = sensor.ReadingChanged(OnLightSensorReadingChanged);
		Subscribed = TRUE;
	}

	AlreadySetup = TRUE;
}

VOID SetupAutoBrightness(SERVICE_STATUS_HANDLE g_StatusHandle)
{
	DWORD type = REG_DWORD, size = 8;

	//
	// Check if brightness is enabled
	//
	DWORD enabled = 0;
	RegQueryValueEx(autoBrightnessKey, L"IsAutobrightnessOn", NULL, &type, (LPBYTE)&enabled, &size);

	if (enabled == 1 && AlreadySetup == FALSE)
	{
		RegisterEverything(g_StatusHandle);
	}
	else if (enabled == 0 && AlreadySetup == TRUE)
	{
		UnregisterEverything();
	}
}

HRESULT BrightnessSensorController::Initialize(SERVICE_STATUS_HANDLE g_StatusHandle)
{
	//InitializeDisplayEnhancementManagement();

	//
	// If no sensor is found return 1
	//
	if (sensor == NULL)
	{
		return 1;
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINDOWS_AUTO_BRIGHTNESS_KEY_PATH, NULL, KEY_READ, &autoBrightnessKey) == ERROR_SUCCESS)
	{
		//
		// Update current brightness on startup
		//
		float currentlux = sensor.GetCurrentReading().IlluminanceInLux();
		int requestedBrightness = floor(currentlux / 10);

		if (requestedBrightness > 100)
			requestedBrightness = 100;

		previousstate = requestedBrightness;
		UpdateDisplayBrightness(requestedBrightness);

		SetupAutoBrightness(g_StatusHandle);

		HANDLE hEvent = CreateEvent(NULL, true, false, NULL);

		RegNotifyChangeKeyValue(autoBrightnessKey, true, REG_NOTIFY_CHANGE_LAST_SET, hEvent, true);

		while (true)
		{
			if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
			{
				break;
			}

			SetupAutoBrightness(g_StatusHandle);

			RegNotifyChangeKeyValue(autoBrightnessKey, false, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_SECURITY, hEvent, true);
		}
	}
	else
	{
		RegisterEverything(g_StatusHandle);

		// Wait indefinetly
		while (true)
		{
			Sleep(4294967295U);
		}
	}

	UnregisterEverything();

	return ERROR_SUCCESS;
}