#include "WCOSWorkarounds.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <map>
#include "Utilities.h"
#include <winreg.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

HRESULT WCOSWorkarounds::Initialize()
{
	HKEY CreatedKey = NULL;
	DWORD disposition;

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		DWORD value = 0;
		RegSetValueEx(CreatedKey, "ConfigFlags", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));

		CONST CHAR* strval = "3&24fd9f5c&0";
		RegSetValueEx(CreatedKey, "ParentIdPrefix", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Device Parameters", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		CONST CHAR* strval = "{AEBF3630-5DC1-11EB-9AAA-806E6F6E6963}";
		RegSetValueEx(CreatedKey, "VideoID", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		strval = "{AEBF3639-5DC1-11EB-9AAA-806E6F6E6963}";
		RegSetValueEx(CreatedKey, "AOCID", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Driver Parameters", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		CONST CHAR* strval = "Adreno 430";
		RegSetValueEx(CreatedKey, "HardwareInformation.AdapterString", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		strval = "Version 06.12";
		RegSetValueEx(CreatedKey, "HardwareInformation.BiosString", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		strval = "InternalDAC";
		RegSetValueEx(CreatedKey, "HardwareInformation.DacType", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		strval = "Adreno 430";
		RegSetValueEx(CreatedKey, "HardwareInformation.ChipType", 0, REG_SZ, (BYTE*)strval, ((DWORD)strlen(strval) + 1) * sizeof(char));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0002", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xBF, 0xB5, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
		RegSetValueEx(CreatedKey, "", 0, 0xffff0009, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{60b193cb-5276-4d0f-96fc-f173abad3ec6}\\0003", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0x00, 0x00, 0x00, 0x00 };
		RegSetValueEx(CreatedKey, "", 0, 0xffff0007, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0009", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0064", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, "", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0065", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, "", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "DEVICES\\ControlSet001\\Enum\\ACPI\\QCOM246F\\0\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\0066", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		BYTE Buffer[] = { 0xF8, 0xB3, 0x15, 0x7C, 0xCE, 0xF1, 0xD6, 0x01 };
		RegSetValueEx(CreatedKey, "", 0, 0xffff0010, (BYTE*)Buffer, sizeof(Buffer));

		RegCloseKey(CreatedKey);
	}

	if (NT_SUCCESS(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Edge", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &CreatedKey, &disposition)))
	{
		DWORD value = 0;
		RegSetValueEx(CreatedKey, "HardwareAccelerationModeEnabled", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));

		RegCloseKey(CreatedKey);
	}

	return ERROR_SUCCESS;
}