#include <Windows.h>
#include "WNFHandler.h"
#include <iostream>
#include "Utilities.h"

extern "C"
{
	NTSTATUS NTAPI RtlPublishWnfStateData(
		_In_ WNF_STATE_NAME StateName,
		_In_opt_ PCWNF_TYPE_ID TypeId,
		_In_reads_bytes_opt_(Length) const VOID* Buffer,
		_In_opt_ ULONG Length,
		_In_opt_ const PVOID ExplicitScope);

	NTSTATUS NTAPI NtQueryWnfStateData(
		_In_ PULONG64 StateName,
		_In_opt_ PWNF_TYPE_ID TypeId,
		_In_opt_ const VOID* ExplicitScope,
		_Out_ PWNF_CHANGE_STAMP ChangeStamp,
		_Out_writes_bytes_to_opt_(*BufferSize, *BufferSize) PVOID Buffer,
		_Inout_ PULONG BufferSize);
}

GUID StringToGuid(const wchar_t* str)
{
	GUID guid;
	int ret = swscanf_s(str,
		L"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
		&guid.Data1, &guid.Data2, &guid.Data3,
		&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
		&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);

	return guid;
}

wchar_t guid_cstr[39];

wchar_t* GuidToString(GUID guid)
{
	swprintf_s(guid_cstr, sizeof(guid_cstr),
		L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return guid_cstr;
}

GUID GetICanGUIDFromConfigurableRegistry(DWORD dwCan)
{
	HKEY hKey;
	LSTATUS nResult = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\OEM\\RILINITSVC", 0, KEY_READ | KEY_WOW64_64KEY, &hKey);

	const wchar_t* ICanStr = L"{000000A0-1087-FF02-33FF-FF89051100FF}";
	DWORD cbData = sizeof(ICanStr);

	bool Logging = FALSE;

	if (nResult == ERROR_SUCCESS)
	{
		if (dwCan == 1)
		{
			nResult = ::RegQueryValueEx(hKey, L"ICan1", NULL, NULL, (LPBYTE)ICanStr, &cbData);
		}
		else
		{
			nResult = ::RegQueryValueEx(hKey, L"ICan0", NULL, NULL, (LPBYTE)ICanStr, &cbData);
		}

		if (nResult != ERROR_SUCCESS)
			ICanStr = L"{000000A0-1087-FF02-33FF-FF89051100FF}";

		RegCloseKey(hKey);
	}

	return StringToGuid(ICanStr);
}

void WNFHandler::SetConfiguredLineDataICanInConfigurableRegistry(DWORD dwCan)
{
	WNF_CELL_CAN_CONFIGURATION_TYPE ConfigurationType = { 0 };
	DWORD cbSize = sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE);
	RtlZeroMemory(&ConfigurationType, cbSize);

	WNF_CHANGE_STAMP wnf_change_stamp;

	WNF_STATE_NAME stateName = WNF_CELL_CONFIGURED_LINES_CAN0;
	if (dwCan == 1)
	{
		stateName = WNF_CELL_CONFIGURED_LINES_CAN1;
	}

	NTSTATUS status = NtQueryWnfStateData(&stateName, nullptr, nullptr, &wnf_change_stamp,
		&ConfigurationType, &cbSize);

	if (status == ERROR_SUCCESS)
	{
		if (ConfigurationType.cLinesIds > 0)
		{
			GUID ConfiguredLine = ConfigurationType.rgbLineIds[0].ICan[0];
			wchar_t* ConfiguredLineStr = GuidToString(ConfiguredLine);

			if (dwCan == 1)
			{
				Utilities::RegSetValueAndCreateKeyIfNeeded(L"SOFTWARE\\OEM\\RILINITSVC", L"ICan1", ConfiguredLineStr);
			}
			else
			{
				Utilities::RegSetValueAndCreateKeyIfNeeded(L"SOFTWARE\\OEM\\RILINITSVC", L"ICan0", ConfiguredLineStr);
			}
		}
	}
}

void WNFHandler::WriteConfiguredLineData(DWORD dwCan, BYTE* ICCID)
{
	GUID ConfiguredICan = GetICanGUIDFromConfigurableRegistry(dwCan);

	WNF_CELL_CAN_CONFIGURATION_TYPE ConfigurationType = { 0 };
	RtlZeroMemory(&ConfigurationType, sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE));

	ConfigurationType.cbSize = sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE);
	memcpy(ConfigurationType.rgbConfiguredIccid, ICCID, 10);

	// Initialize one ICan
	ConfigurationType.cLinesIds = 1;
	memcpy(ConfigurationType.rgbLineIds[0].rgbConfiguredIccid, ICCID, 10);
	ConfigurationType.rgbLineIds[0].Unknown = 0x05;
	memcpy(&ConfigurationType.rgbLineIds[0].ICan[0], &ConfiguredICan, sizeof(GUID));

	WNF_STATE_NAME stateName = WNF_CELL_CONFIGURED_LINES_CAN0;
	if (dwCan == 1)
	{
		stateName = WNF_CELL_CONFIGURED_LINES_CAN1;
	}

	HRESULT nError = RtlPublishWnfStateData(stateName, nullptr, &ConfigurationType, 0x74, nullptr);

	if (nError != ERROR_SUCCESS)
	{
		std::cout << "Failed to write WNF line registration information for phone service using NtUpdateWnfStateData. hResult=" << std::hex << nError << std::endl;
	}
}

void WNFHandler::WriteBlankConfiguredLineData(DWORD dwCan)
{
	WNF_CELL_CAN_CONFIGURATION_TYPE ConfigurationType = { 0 };
	RtlZeroMemory(&ConfigurationType, sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE));

	ConfigurationType.cbSize = sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE);
	ConfigurationType.dwSlotAffinity = 1;

	WNF_STATE_NAME stateName = WNF_CELL_CONFIGURED_LINES_CAN0;
	if (dwCan == 1)
	{
		stateName = WNF_CELL_CONFIGURED_LINES_CAN1;
	}

	HRESULT nError = RtlPublishWnfStateData(stateName, nullptr, &ConfigurationType, sizeof(WNF_CELL_CAN_CONFIGURATION_TYPE), nullptr);

	if (nError != ERROR_SUCCESS)
	{
		std::cout << "Failed to write WNF line registration information for phone service using NtUpdateWnfStateData. hResult=" << std::hex << nError << std::endl;
	}
}
