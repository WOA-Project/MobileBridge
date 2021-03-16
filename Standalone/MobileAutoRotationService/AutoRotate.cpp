#include "pch.h"
#include "AutoRotate.h"
#include "NtAlpc.h"

CRITICAL_SECTION g_AutoRotationCriticalSection;

//
// Subject: Notify auto rotation with the following current auto rotation settings
//			using ALPC port
//
// Parameters:
//
//			   Handle: the ALPC Port Handle
//
//             Orientation:
//             - DMDO_270     (Portrait)
//             - DMDO_90      (Portrait flipped)
//             - DMDO_180     (Landscape)
//             - DMDO_DEFAULT (Landscape flipped)
//
// Returns: NTSTATUS
//
NTSTATUS NotifyAutoRotateAlpcPort(HANDLE PortHandle, INT Orientation)
{
	//UCHAR RotationMessage[56];
	ROTATION_COMMAND_MESSAGE RotationCommandMessage;
	NTSTATUS Status;

	if (PortHandle == NULL) return STATUS_INVALID_PARAMETER;

//#if !defined(_M_ARM64) && !defined(_M_X64)
//	// Only 64bit OS is currently validated
//	return 0xC00000BB;
//#endif

	EnterCriticalSection(&g_AutoRotationCriticalSection);

	/*RtlZeroMemory(&RotationMessage, sizeof(RotationMessage));
	*(unsigned short*)&RotationMessage = 16;
	*(unsigned short*)&RotationMessage[2] = 56;
	*(unsigned short*)&RotationMessage[4] = 1;
	*(unsigned int*)&RotationMessage[40] = 2;
	*(unsigned int*)&RotationMessage[52] = Orientation;*/

	RtlZeroMemory(&RotationCommandMessage, sizeof(RotationCommandMessage));

	RotationCommandMessage.PortMessage.u1.s1.DataLength = sizeof(RotationCommandMessage.RotationMessage);
	RotationCommandMessage.PortMessage.u1.s1.TotalLength = sizeof(RotationCommandMessage);
	RotationCommandMessage.PortMessage.u2.s2.Type = 1;

	RotationCommandMessage.RotationMessage.Type = 2;
	RotationCommandMessage.RotationMessage.Orientation = Orientation;

	Status = NtAlpcSendWaitReceivePort(PortHandle, 0, (PVOID)&RotationCommandMessage, NULL, NULL, NULL, NULL, NULL);

	LeaveCriticalSection(&g_AutoRotationCriticalSection);

	return Status;
}

//
// Subject: Change screen orientation while following current auto rotation settings
//
// Parameters:
//
//			   Handle: the ALPC Port Handle
//
//             Orientation:
//             - DMDO_270     (Portrait)
//             - DMDO_90      (Portrait flipped)
//             - DMDO_180     (Landscape)
//             - DMDO_DEFAULT (Landscape flipped)
//
// Returns: void
//
VOID ChangeDisplayOrientation(HANDLE PortHandle, INT Orientation)
{
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AutoRotation", NULL, KEY_READ, &key) == ERROR_SUCCESS)
	{
		DWORD type = REG_DWORD, size = 8;

		//
		// Check if we are supposed to use the mobile behavior, if we do, then prevent the screen from rotating to Portrait (flipped)
		//
		if (Orientation == DMDO_90)
		{
			DWORD mobilebehavior = 0;
			RegQueryValueEx(key, L"MobileBehavior", NULL, &type, (LPBYTE)&mobilebehavior, &size);
			if (mobilebehavior == 1)
			{
				return;
			}
		}

		//
		// Check if rotation is enabled
		//
		DWORD enabled = 0;
		RegQueryValueEx(key, L"Enable", NULL, &type, (LPBYTE)&enabled, &size);

		//
		// Check if new API is available
		//
		if (enabled == 1 && PortHandle != NULL)
		{
			NotifyAutoRotateAlpcPort(PortHandle, Orientation);
		}
		// Otherwise fallback to the old API
		else if (enabled == 1)
		{
			//
			// Get the current display settings
			//
			DEVMODE mode;
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);

			//
			// In order to switch from portrait to landscape and vice versa we need to swap the resolution width and height
			// So we check for that
			//
			if ((mode.dmDisplayOrientation + Orientation) % 2 == 1)
			{
				int temp = mode.dmPelsHeight;
				mode.dmPelsHeight = mode.dmPelsWidth;
				mode.dmPelsWidth = temp;
			}

			//
			// Change the display orientation and save to the registry the changes (1 parameter for ChangeDisplaySettings)
			//
			if (mode.dmFields | DM_DISPLAYORIENTATION)
			{
				mode.dmDisplayOrientation = Orientation;
				ChangeDisplaySettingsEx(NULL, &mode, NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL, NULL);
			}
		}
	}
}

int AutoRotateMain()
{
	NTSTATUS Status;
	UNICODE_STRING DestinationString;
	HANDLE PortHandle = NULL;
	OBJECT_ATTRIBUTES ObjectAttribs;
	ALPC_PORT_ATTRIBUTES PortAttribs = { 0 };

	//
	// Get the default simple orientation sensor on the system
	//
	SimpleOrientationSensor sensor = SimpleOrientationSensor::GetDefault();

	//
	// If no sensor is found return 1
	//
	if (sensor == NULL)
	{
		return 1;
	}

	//
	// Initialize ALPC Port. This will decide the way we do screen flip
	//
	RtlZeroMemory(&ObjectAttribs, sizeof(ObjectAttribs));
	ObjectAttribs.Length = sizeof(ObjectAttribs);

	RtlZeroMemory(&PortAttribs, sizeof(PortAttribs));
	PortAttribs.MaxMessageLength = 56;

	RtlInitUnicodeString(&DestinationString, L"\\RPC Control\\AutoRotationApiPort");
#pragma warning(disable:6387)
	Status = NtAlpcConnectPort(&PortHandle, &DestinationString, &ObjectAttribs, &PortAttribs, ALPC_MSGFLG_SYNC_REQUEST, NULL, NULL, NULL, NULL, NULL, NULL);
#pragma warning(default:6387)
	if (NT_SUCCESS(Status)) {
		// Initialize the critical section one time only.
		if (!InitializeCriticalSectionAndSpinCount(&g_AutoRotationCriticalSection, 0x00000400))
		{
			// Can't initialize critical section; skip Auto rotation API
			PortHandle = NULL;
		}
	}
	else {
		// Reset it anyway
		PortHandle = NULL;
	}

	//
	// Set sensor present for windows to show the auto rotation toggle in action center
	//
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AutoRotation", NULL, KEY_WRITE, &key) == ERROR_SUCCESS)
	{
		RegSetValueEx(key, L"SensorPresent", NULL, REG_DWORD, (LPBYTE)1, 8);
	}

	//
	// Subscribe to sensor events
	//
	sensor.OrientationChanged([PortHandle](IInspectable const& /*sender*/, SimpleOrientationSensorOrientationChangedEventArgs const& args)
		{
			switch (args.Orientation())
			{
			// Portrait
			case SimpleOrientation::NotRotated:
			{
				ChangeDisplayOrientation(PortHandle, DMDO_270);
				break;
			}
			// Portrait (flipped)
			case SimpleOrientation::Rotated180DegreesCounterclockwise:
			{
				ChangeDisplayOrientation(PortHandle, DMDO_90);
				break;
			}
			// Landscape
			case SimpleOrientation::Rotated90DegreesCounterclockwise:
			{
				ChangeDisplayOrientation(PortHandle, DMDO_180);
				break;
			}
			// Landscape (flipped)
			case SimpleOrientation::Rotated270DegreesCounterclockwise:
			{
				ChangeDisplayOrientation(PortHandle, DMDO_DEFAULT);
				break;
			}
			}
		});

	// Wait indefinetly
	while (true)
	{
		Sleep(4294967295U);
	}

	// Close handle for the future
	// Even the thread doesn't support exit right now
	if (PortHandle != NULL) {
		NtClose(PortHandle);
		DeleteCriticalSection(&g_AutoRotationCriticalSection);
	}

	return 0;
}
