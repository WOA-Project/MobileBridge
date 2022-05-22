#include "pch.h"
#include "HapticsNotifications.h"

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
NTSTATUS NTAPI WnfCallback(ULONGLONG, PVOID, PVOID, PVOID, PVOID, PVOID);


typedef struct _WNF_TYPE_ID
{
	GUID TypeId;
} WNF_TYPE_ID, * PWNF_TYPE_ID;
typedef const WNF_TYPE_ID* PCWNF_TYPE_ID;

typedef ULONG WNF_CHANGE_STAMP, * PWNF_CHANGE_STAMP;

extern "C" {
	NTSTATUS NTAPI RtlSubscribeWnfStateChangeNotification(PVOID, ULONGLONG, UINT, decltype(WnfCallback), size_t, size_t, size_t, size_t);
	NTSTATUS NTAPI RtlUnsubscribeWnfStateChangeNotification(decltype(WnfCallback));

	NTSTATUS NTAPI
		NtQueryWnfStateData(
			_In_ PULONG64 StateName,
			_In_opt_ PWNF_TYPE_ID TypeId,
			_In_opt_ const VOID* ExplicitScope,
			_Out_ PWNF_CHANGE_STAMP ChangeStamp,
			_Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
			_Inout_ PULONG BufferSize);
}

static SimpleHapticsController controller = NULL;
static SimpleHapticsControllerFeedback controllerFeedback = NULL;
ULONG64 WNF_SHEL_TOAST_PUBLISHED = 0xD83063EA3BD0035;

BOOL GetState()
{
	HKEY key;
	DWORD hr;
	BOOL enabled = TRUE;

	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\OEM\\Vibra", NULL, KEY_READ, &key);
	if (hr == ERROR_SUCCESS)
	{
		BOOL state = TRUE;
		DWORD type = REG_DWORD, size = 8;
		hr = RegQueryValueEx(key, L"Enabled", NULL, &type, (LPBYTE)&state, &size);

		if (hr != ERROR_SUCCESS)
		{
			goto CLEANUP;
		}

		if (type == REG_DWORD)
		{
			enabled = state;
		}
	CLEANUP:
		RegCloseKey(key);
	}

	return enabled;
}

ULONG GetHapticsDuration()
{
	HKEY key;
	DWORD hr;
	ULONG time = 5000000L;

	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\OEM\\Vibra", NULL, KEY_READ, &key);
	if (hr == ERROR_SUCCESS)
	{
		unsigned long tms = 5;
		unsigned long type = REG_DWORD, size = 8;
		hr = RegQueryValueEx(key, L"Duration", NULL, &type, (LPBYTE)&tms, &size);

		if (hr != ERROR_SUCCESS)
		{
			goto CLEANUP;
		}

		if (type == REG_DWORD)
		{
			if (tms > 10)
				tms = 10;
			else if (tms < 1)
				tms = 1;

			time = tms * 1000000L;
		}
	CLEANUP:
		RegCloseKey(key);
	}

	return time;
}

UINT GetHapticsIntensity()
{
	HKEY key;
	DWORD hr;
	UINT intensity = 100;

	hr = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\OEM\\Vibra", NULL, KEY_READ, &key);
	if (hr == ERROR_SUCCESS)
	{
		UINT ints = 100u;
		DWORD type = REG_DWORD, size = 8;
		hr = RegQueryValueEx(key, L"Intensity", NULL, &type, (LPBYTE)&ints, &size);

		if (hr != ERROR_SUCCESS)
		{
			goto CLEANUP;
		}

		if (type == REG_DWORD)
		{
			if (ints > 100)
				ints = 100;
			else if (ints < 20)
				ints = 20;

			intensity = ints;
		}
	CLEANUP:
		RegCloseKey(key);
	}

	return intensity;
}

NTSTATUS NTAPI WnfCallback(ULONGLONG p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5, PVOID p6)
{
	if (GetState())
	{
		UINT intensity = GetHapticsIntensity();
		ULONG time = GetHapticsDuration();

		if (controller != NULL)
		{
			try {
				TimeSpan span(time);
				controller.SendHapticFeedbackForDuration(controllerFeedback, ((double)intensity / (double)100), span);
			}
			catch (...) {
				Sleep(time * 0.0001);
				try {
					controller.StopFeedback();
				}
				catch (...) {
				}
			}
		}
	}
	return 0;
}

INT HapticsNotificationsMain()
{
	// Acquires access to haptics hardware
	VibrationAccessStatus auth{ VibrationDevice::RequestAccessAsync().get() };

	if (auth == VibrationAccessStatus::Allowed)
	{
		VibrationDevice device = VibrationDevice::GetDefaultAsync().get();
		if (device != NULL)
		{
			controller = device.SimpleHapticsController();
			controllerFeedback = controller.SupportedFeedback().GetAt(0);

			uint32_t buf1{};
			size_t buf2{};

			//This part (Query wnf) is needed to prevent from vibrating at subscription by providing the newest changestamp
			std::vector<unsigned char> wnf_state_buffer(8192);
			unsigned long state_buffer_size = 8192;
			WNF_CHANGE_STAMP wnf_change_stamp;

			NtQueryWnfStateData(&WNF_SHEL_TOAST_PUBLISHED, nullptr, nullptr, &wnf_change_stamp,
				wnf_state_buffer.data(), &state_buffer_size);
			
			NTSTATUS result = RtlSubscribeWnfStateChangeNotification(&buf2, WNF_SHEL_TOAST_PUBLISHED, wnf_change_stamp, WnfCallback, 0, 0, 0, 1);

			if (result == ERROR_SUCCESS)
			{
				//Signal subscribing succeeded for debugging purposes
				//TimeSpan span(10000000L);
				//controller.SendHapticFeedbackForDuration(controllerFeedback, 1, span);
			}
			else
			{
				//Quit thread, we should log this eventually
				return 0;
			}

			// Wait indefinetly

			while (true)
			{
				Sleep(4294967295U);
			}
		}
	}

	return 0;
}