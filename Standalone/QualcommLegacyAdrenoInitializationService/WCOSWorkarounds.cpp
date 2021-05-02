#include "WCOSWorkarounds.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <map>
#include <winreg.h>

#define STATE_SEPARATION_PATH L"DEVICES\\ControlSet001\\"
#define NORMAL_PATH L"SYSTEM\\CurrentControlSet\\"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

HRESULT WCOSWorkarounds::Initialize()
{
	HKEY CreatedKey = NULL;
	DWORD disposition;

	HKEY DevicesKey = NULL;
	BOOL StateSeparated = NT_SUCCESS(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"DEVICES\\ControlSet001", 0, KEY_ALL_ACCESS, &DevicesKey));
	if (StateSeparated && DevicesKey != NULL)
	{
		RegCloseKey(DevicesKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		DWORD value = 0;
		RegSetValueEx(CreatedKey, L"ConfigFlags", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));

		CONST WCHAR* strval = L"3&24fd9f5c&0";
		RegSetValueEx(CreatedKey, L"ParentIdPrefix", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Device Parameters") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Device Parameters"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		CONST WCHAR* strval = L"{AEBF3630-5DC1-11EB-9AAA-806E6F6E6963}";
		RegSetValueEx(CreatedKey, L"VideoID", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		strval = L"{AEBF3639-5DC1-11EB-9AAA-806E6F6E6963}";
		RegSetValueEx(CreatedKey, L"AOCID", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Driver Parameters") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Driver Parameters"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		CONST WCHAR* strval = L"Adreno 430";
		RegSetValueEx(CreatedKey, L"HardwareInformation.AdapterString", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		strval = L"Version 06.12";
		RegSetValueEx(CreatedKey, L"HardwareInformation.BiosString", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		strval = L"InternalDAC";
		RegSetValueEx(CreatedKey, L"HardwareInformation.DacType", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		strval = L"Adreno 430";
		RegSetValueEx(CreatedKey, L"HardwareInformation.ChipType", 0, REG_SZ, (BYTE*)strval, ((DWORD)wcslen(strval) + 1) * sizeof(wchar_t));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0002") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0002"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xBF, 0xB5, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
		RegSetValueEx(CreatedKey, L"", 0, 0xffff0009, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0003") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0003"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0x00, 0x00, 0x00, 0x00 };
		RegSetValueEx(CreatedKey, L"", 0, 0xffff0007, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0009") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0009"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0064") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0064"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, L"", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0065") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0065"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, L"", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, StateSeparated ? (STATE_SEPARATION_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0066") : (NORMAL_PATH L"Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0066"), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, L"", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Policies\\Microsoft\\Edge", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		DWORD value = 0;
		RegSetValueEx(CreatedKey, L"HardwareAccelerationModeEnabled", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));

		RegCloseKey(CreatedKey);
	}

	return ERROR_SUCCESS;
}