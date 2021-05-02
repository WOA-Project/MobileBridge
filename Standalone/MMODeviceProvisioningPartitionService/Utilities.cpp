#include "Utilities.h"
#include <Windows.h>

BOOL Utilities::DoesDirectoryExist(const wchar_t* dirName) {
	DWORD attribs = ::GetFileAttributes(dirName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL Utilities::DoesFileExist(const wchar_t* filName) {
	DWORD attribs = ::GetFileAttributes(filName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return !(attribs & FILE_ATTRIBUTE_DIRECTORY);
}

void Utilities::RegSetValueAndCreateKeyIfNeeded(const wchar_t* key, const wchar_t* valueName, const wchar_t* value)
{
	LONG status;
	HKEY hKey;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_ALL_ACCESS, &hKey);
	if ((status == ERROR_SUCCESS) && (hKey != NULL))
	{
		status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)value, ((DWORD)wcslen(value) + 1) * sizeof(wchar_t));
		RegCloseKey(hKey);
	}
	else
	{
		status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
		if ((status == ERROR_SUCCESS) && (hKey != NULL))
		{
			status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)value, ((DWORD)wcslen(value) + 1) * sizeof(wchar_t));
			RegCloseKey(hKey);
		}
	}
}