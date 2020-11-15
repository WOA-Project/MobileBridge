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

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Sensors;
using namespace winrt::Windows::Internal::Graphics::Display::DisplayEnhancementManagement;

DisplayEnhancementManagement dispEnhMan = NULL;
LightSensor sensor = NULL;

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

	typedef struct _DISPLAY_BRIGHTNESS {
		UCHAR ucDisplayPolicy;
		UCHAR ucACBrightness;
		UCHAR ucDCBrightness;
	} DISPLAY_BRIGHTNESS, * PDISPLAY_BRIGHTNESS;

	DISPLAY_BRIGHTNESS _displayBrightness;
	DWORD nOutBufferSize = sizeof(_displayBrightness);
	HANDLE h = NULL;

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\DisplayEnhancementService\\State\\CMO00110_09_07D9_98", 0, KEY_NOTIFY | KEY_READ, &displayKey);

	RegQueryValueEx(displayKey, "IsAutobrightnessOn", NULL, &type, (LPBYTE)&IsAutobrightnessOn, &size);

	RegCloseKey(displayKey);

	//if (!dispEnhMan.IsAutobrightnessOn())
	if (!IsAutobrightnessOn)
	{
		return;
	}

	_displayBrightness.ucDisplayPolicy = 0;
	_displayBrightness.ucACBrightness = brightness;
	_displayBrightness.ucDCBrightness = brightness;

	h = CreateFile("\\\\.\\LCD",
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
	WCHAR uwu[128]{};

	EnumDisplayDevices(0, 0, &dd, 0);
	std::string deviceName = dd.DeviceName;
	EnumDisplayDevices(deviceName.c_str(), 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
	mbstowcs_s(&outSize, uwu, dd.DeviceID, 128);
	//dispEnhMan = DisplayEnhancementManagement::FromIdAsync(uwu).get(); // crashes in service (?)
}

HRESULT BrightnessSensorController::Initialize()
{
	InitializeDisplayEnhancementManagement();

	//
	// Get the default light sensor on the system
	//
	sensor = LightSensor::GetDefault();

	//
	// If no sensor is found return 1
	//
	if (sensor == NULL)
	{
		return 1;
	}

	//
	// Update current brightness on startup
	//
	float currentlux = sensor.GetCurrentReading().IlluminanceInLux();
	int requestedBrightness = floor(currentlux / 10);

	if (requestedBrightness > 100)
		requestedBrightness = 100;

	previousstate = requestedBrightness;
	UpdateDisplayBrightness(requestedBrightness);

	//
	// Subscribe to sensor events
	//
	sensor.ReadingChanged([](IInspectable const& /*sender*/, LightSensorReadingChangedEventArgs const& args)
		{
			LightSensorReading readings = args.Reading();
			float lux = readings.IlluminanceInLux();

			int targetBrightness = floor(lux / 10);

			if (targetBrightness > 100)
				targetBrightness = 100;

			printf("Brightness: %d\n", targetBrightness);

			IsRadicallyDifferent(targetBrightness);
		});

	return ERROR_SUCCESS;
}