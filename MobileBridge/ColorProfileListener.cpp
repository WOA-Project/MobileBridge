#include "pch.h"
#include "ColorProfileListener.h"
#include <winreg.h>
#include <vector>
#include <string>
#include <thread>
#include <algorithm>
#include <winerror.h>
#include <WinBase.h>

#define PROFILE_KEY_PATH L"Software\\Microsoft\\Windows NT\\CurrentVersion\\ICM\\ProfileAssociations\\Display\\{4d36e96e-e325-11ce-bfc1-08002be10318}\\0002"
#define COLOR_AND_LIGHT_KEY_PATH L"SOFTWARE\\OEM\\Nokia\\Display\\ColorAndLight"

const wchar_t* CS_blueLightState = L"windows.data.bluelightreduction.bluelightreductionstate";
const wchar_t* CS_blueLightSettings = L"windows.data.bluelightreduction.settings";

// Back store to prevent multiple signaling.
BOOL _nightLight = FALSE; //Default value.
BOOL _isEnabled = FALSE; //Default value. This is another backstore.
BOOL _isPreviewing = FALSE; //Default value.
BOOL _loaded = FALSE; //Default value.
INT _colorTemperature = 6500; //6500K is the default/standard white point
std::wstring _lastFoundProfile = std::wstring(L""); //Default value.

HKEY profileKey = NULL;
HKEY colorAndLightKey = NULL;

std::wstring ReadSelectedProfile()
{
	DWORD type, size;
	std::vector<std::wstring> target;
	int count = -1;

	LSTATUS nError = RegQueryValueExW(profileKey, L"ICMProfile", NULL, &type, NULL, &size);
	if (nError != ERROR_SUCCESS)
	{
		return L"";
	}

	if (type == REG_MULTI_SZ)
	{
		std::vector<WCHAR> temp(size / sizeof(WCHAR));

		nError = RegQueryValueExW(profileKey, L"ICMProfile", NULL, NULL, reinterpret_cast<LPBYTE>(&temp[0]), &size);
		if (nError != ERROR_SUCCESS)
		{
			return L"";
		}

		ULONG64 index = 0;
		ULONG64 len = wcslen(&temp[0]);

		while (len > 0)
		{
			target.push_back(&temp[index]);
			index += len + 1;
			len = wcslen(&temp[index]);
			count++;
		}
	}

	if (count != -1)
	{
		return target[count];
	}

	return L"";
}

VOID ProvisionDefaultProfileData()
{
	WCHAR buf[255] = { 0 };
	ULONG dwType = 0;
	ULONG dwBufSize = sizeof(buf);

	HKEY hProfileKey;
	LSTATUS nError = RegOpenKeyEx(HKEY_CURRENT_USER, PROFILE_KEY_PATH, NULL, KEY_ALL_ACCESS, &hProfileKey);

	if (nError == ERROR_FILE_NOT_FOUND)
	{
		RegCreateKeyEx(HKEY_CURRENT_USER, PROFILE_KEY_PATH, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hProfileKey, NULL);

		const wchar_t sz[] = L"Advanced.icm\0Cool.icm\0Vivid.icm\0Standard.icm\0";

		RegSetValueEx(hProfileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		RegSetValueEx(hProfileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));

		RegSetValueEx(hProfileKey, L"ICMProfileAC", NULL, REG_MULTI_SZ, NULL, 0);
		RegSetValueEx(hProfileKey, L"ICMProfileSnapshot", NULL, REG_MULTI_SZ, NULL, 0);
		RegSetValueEx(hProfileKey, L"ICMProfileSnapshotAC", NULL, REG_MULTI_SZ, NULL, 0);

		ULONG data = 1;
		RegSetValueEx(hProfileKey, L"UsePerUserProfiles", NULL, REG_DWORD, (LPBYTE)&data, sizeof(unsigned long));
	}

	dwBufSize = 0;
	RegQueryValueEx(hProfileKey, L"ICMProfile", NULL, &dwType, (BYTE*)buf, &dwBufSize);
	if (dwBufSize == 0)
	{
		CONST WCHAR sz[] = L"Advanced.icm\0Cool.icm\0Vivid.icm\0Standard.icm\0";

		RegSetValueEx(hProfileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		RegSetValueEx(hProfileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));

		RegSetValueEx(hProfileKey, L"ICMProfileAC", NULL, REG_MULTI_SZ, NULL, 0);
		RegSetValueEx(hProfileKey, L"ICMProfileSnapshot", NULL, REG_MULTI_SZ, NULL, 0);
		RegSetValueEx(hProfileKey, L"ICMProfileSnapshotAC", NULL, REG_MULTI_SZ, NULL, 0);

		DWORD data = 1;
		RegSetValueEx(hProfileKey, L"UsePerUserProfiles", NULL, REG_DWORD, (LPBYTE)&data, sizeof(DWORD));
	}

	RegCloseKey(hProfileKey);
}

VOID ProvisionProfileListData()
{
	WCHAR buf[255] = { 0 };
	DWORD dwType = 0;
	DWORD dwBufSize = sizeof(buf);

	RegQueryValueEx(colorAndLightKey, L"UserSettingSelectedProfile", NULL, &dwType, (BYTE*)buf, &dwBufSize);

	std::wstring profile = std::wstring(buf);

	if (profile == L"Night light.icm")
	{
		_nightLight = true;

		const wchar_t sz[] = L"Night light.icm\0";
		RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));

		return;
	}
	else
	{
		_nightLight = false;
	}

	if (!profile.empty())
	{
		if (profile == _lastFoundProfile)
		{
			return;
		}

		_lastFoundProfile = profile;

		if (profile == L"Standard.icm")
		{
			CONST WCHAR sz[] = L"Advanced.icm\0Cool.icm\0Vivid.icm\0Standard.icm\0";

			RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
			RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		}
		else if (profile == L"Vivid.icm")
		{
			CONST WCHAR sz[] = L"Standard.icm\0Advanced.icm\0Cool.icm\0Vivid.icm\0";

			RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
			RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		}
		else if (profile == L"Cool.icm")
		{
			CONST WCHAR sz[] = L"Vivid.icm\0Standard.icm\0Advanced.icm\0Cool.icm\0";

			RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
			RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		}
		else if (profile == L"Advanced.icm")
		{
			CONST WCHAR sz[] = L"Cool.icm\0Vivid.icm\0Standard.icm\0Advanced.icm\0";

			RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
			RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
		}
	}
}

VOID CheckForProfileChangeFromExternal()
{
	HANDLE hEvent = CreateEvent(NULL, true, false, NULL);

	RegNotifyChangeKeyValue(colorAndLightKey, true, REG_NOTIFY_CHANGE_LAST_SET, hEvent, true);

	while (true)
	{
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
		{
			exit(0);
		}

		ProvisionProfileListData();

		RegNotifyChangeKeyValue(colorAndLightKey, false, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_SECURITY, hEvent, true);
	}
}

VOID CheckForProfileChangeFromInternal()
{
	HANDLE hEvent = CreateEvent(NULL, true, false, NULL);

	RegNotifyChangeKeyValue(profileKey, true, REG_NOTIFY_CHANGE_LAST_SET, hEvent, true);

	while (true)
	{
		if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
		{
			exit(0);
		}

		if (!_nightLight)
		{
			std::wstring tmpstring = ReadSelectedProfile();

			if (tmpstring == L"Night light.icm")
			{
				_nightLight = true;

				const wchar_t sz[] = L"Night light.icm\0";
				RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));

				continue;
			}

			if (_wcsicmp(_lastFoundProfile.c_str(), tmpstring.c_str()))
			{
				_lastFoundProfile = tmpstring;

				if (_lastFoundProfile == L"Standard.icm")
				{
					const wchar_t sz[] = L"Advanced.icm\0Cool.icm\0Vivid.icm\0Standard.icm\0";
					RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
				}
				if (_lastFoundProfile == L"Vivid.icm")
				{
					const wchar_t sz[] = L"Standard.icm\0Advanced.icm\0Cool.icm\0Vivid.icm\0";
					RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
				}
				if (_lastFoundProfile == L"Cool.icm")
				{
					const wchar_t sz[] = L"Vivid.icm\0Standard.icm\0Advanced.icm\0Cool.icm\0";
					RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
				}
				if (_lastFoundProfile == L"Advanced.icm")
				{
					const wchar_t sz[] = L"Cool.icm\0Vivid.icm\0Standard.icm\0Advanced.icm\0";
					RegSetValueEx(profileKey, L"ICMProfileBackup", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));
				}

				ChangeColorProfile(_lastFoundProfile);
			}
		}

		RegNotifyChangeKeyValue(profileKey, false, REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_SECURITY, hEvent, true);
	}
}

VOID ChangeColorProfileNightLight(double colorTemperature)
{
	const std::wstring profileName = L"Night light.icm";
	RegSetValueEx(colorAndLightKey, L"UserSettingSelectedProfile", NULL, REG_SZ, LPBYTE(profileName.c_str()), (profileName.size() + 1) * sizeof(wchar_t));

	const Profile prof = Profile::GetNightLightProfile(colorTemperature);
	prof.ApplyProfile(colorAndLightKey);
}

VOID ChangeColorProfile(std::wstring lastprofile)
{
	Profile::SetValue(colorAndLightKey, L"UserSettingSelectedProfile", lastprofile);
	ProvisionProfileListData();

	if (lastprofile == L"Standard.icm")
	{
		const Profile prof = Profile::GetDefault();
		prof.ApplyProfile(colorAndLightKey);
	}
	else if (lastprofile == L"Vivid.icm")
	{
		const Profile prof = Profile::GetVivid();
		prof.ApplyProfile(colorAndLightKey);
	}
	else if (lastprofile == L"Cool.icm")
	{
		const Profile prof = Profile::GetCool();
		prof.ApplyProfile(colorAndLightKey);
	}
	else if (lastprofile == L"Advanced.icm")
	{
		int saturation = 0;
		int tint = 0;
		int temp = 0;

		unsigned long dwData = 0;
		unsigned long cbData = sizeof(unsigned long);

		long err = RegQueryValueEx(colorAndLightKey, L"UserSettingAdvancedSaturation", 0, 0, LPBYTE(&dwData), &cbData);

		if (err == ERROR_SUCCESS)
			saturation = dwData;
		else
			saturation = 25;

		err = RegQueryValueEx(colorAndLightKey, L"UserSettingAdvancedTint", 0, 0, LPBYTE(&dwData), &cbData);

		if (err == ERROR_SUCCESS)
			tint = dwData;
		else
			tint = 50;

		err = RegQueryValueEx(colorAndLightKey, L"UserSettingAdvancedTemperature", 0, 0, LPBYTE(&dwData), &cbData);

		if (err == ERROR_SUCCESS)
			temp = dwData;
		else
			temp = 50;


		const auto prof = Profile::GenerateAdvancedProfile(temp, tint, saturation);
		prof.ApplyProfile(colorAndLightKey);
	}
}

//---------------------------
//NIGHT MODE RELATED SETTINGS

CloudStore cloudStoreInstance = nullptr;
CloudStoreDataWatcher blueLightReductionSettingsWatcher = nullptr, blueLightReductionStateWatcher = nullptr;

VOID CheckForNightLight()
{
	//Preload data first

	cloudStoreInstance = CloudStore::CreateInstance((CloudStoreOptions)((int)CloudStoreOptions::UseCurrentUser | (int)CloudStoreOptions::UseDefaultWebAccount), NULL, NULL, L"");

	auto dataSettings = cloudStoreInstance.Load(PartitionKind::None, L"", L"", L"", CS_blueLightSettings, LoadOptions::LocalOnly, L"");
	auto jsonSettings = cloudStoreInstance.ToJson(CS_blueLightSettings, dataSettings.Data());
	auto strSettings = jsonSettings.GetNamedObject(L"Data");

	auto isPreviewing = strSettings.GetNamedBoolean(L"previewColorTemperatureChanges");
	auto colorTemperature = (INT)strSettings.GetNamedNumber(L"targetColorTemperature");

	auto dataState = cloudStoreInstance.Load(PartitionKind::None, L"", L"", L"", CS_blueLightState, LoadOptions::LocalOnly, L"");
	auto jsonState = cloudStoreInstance.ToJson(CS_blueLightState, dataState.Data());
	auto strState = jsonState.GetNamedObject(L"Data");

	auto isEnabled = 1 - (INT)strState.GetNamedNumber(L"state");

	NewStatus(isEnabled, isPreviewing, colorTemperature);

	//set up watchers

	blueLightReductionSettingsWatcher = cloudStoreInstance.CreateWatcher(PartitionKind::None, L"", L"", L"", CS_blueLightSettings, (CloudDataChangeKinds)((int)CloudDataChangeKinds::CloudDataLoaded | (int)CloudDataChangeKinds::CloudDataChanged), 0, L"");

	blueLightReductionSettingsWatcher.DataChanged([=](CloudStoreDataWatcher sender, CloudStoreDataChangedEventArgs args) {
		auto json = cloudStoreInstance.ToJson(CS_blueLightSettings, args.DataItem().Data());
		auto str = json.GetNamedObject(L"Data");

		auto isPreviewing = str.GetNamedBoolean(L"previewColorTemperatureChanges");
		auto colorTemperature = (INT)str.GetNamedNumber(L"targetColorTemperature");

		NewStatus(_isEnabled, isPreviewing, colorTemperature);
		});

	blueLightReductionStateWatcher = cloudStoreInstance.CreateWatcher(PartitionKind::None, L"", L"", L"", CS_blueLightState, (CloudDataChangeKinds)((int)CloudDataChangeKinds::CloudDataLoaded | (int)CloudDataChangeKinds::CloudDataChanged), 0, L"");

	blueLightReductionStateWatcher.DataChanged([=](CloudStoreDataWatcher sender, CloudStoreDataChangedEventArgs args) {
		auto json = cloudStoreInstance.ToJson(CS_blueLightState, args.DataItem().Data());
		auto str = json.GetNamedObject(L"Data");

		auto isEnabled = 1 - (INT)str.GetNamedNumber(L"state");

		NewStatus(isEnabled, _isPreviewing, _colorTemperature);
		});
}

VOID NewStatus(BOOL isEnabled, BOOL isPreviewing, INT colorTemperature)
{
	if (isEnabled || isPreviewing)
	{
		EnableNightLight(colorTemperature);
	}
	else if (isPreviewing != _isPreviewing || (!isPreviewing && !isEnabled))
	{
		DisableNightLight();
	}

	_isEnabled = isEnabled;
	_colorTemperature = colorTemperature;
	_isPreviewing = isPreviewing;
}

VOID EnableNightLight(ULONG colorTemperature)
{
	_nightLight = true;

	CONST WCHAR sz[] = L"Night light.icm\0";
	RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)sz, sizeof(sz));

	ChangeColorProfileNightLight(colorTemperature);
}

VOID DisableNightLight()
{
	wchar_t buf[255] = { 0 };
	DWORD dwType = 0;
	DWORD dwBufSize = sizeof(buf);

	RegQueryValueEx(profileKey, L"ICMProfileBackup", NULL, &dwType, (BYTE*)buf, &dwBufSize);
	RegSetValueEx(profileKey, L"ICMProfile", NULL, REG_MULTI_SZ, (LPBYTE)buf, dwBufSize);

	std::wstring tmpstring = ReadSelectedProfile();

	_lastFoundProfile = tmpstring;
	ChangeColorProfile(_lastFoundProfile);

	_nightLight = false;
}

//-------------------------
// MAIN

int ColorProfileListenerMain()
{
	ProvisionDefaultProfileData();

	RegOpenKeyEx(HKEY_CURRENT_USER, PROFILE_KEY_PATH, 0, KEY_ALL_ACCESS, &profileKey);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, COLOR_AND_LIGHT_KEY_PATH, 0, KEY_ALL_ACCESS, &colorAndLightKey);

	CheckForNightLight();

	std::thread t3(CheckForProfileChangeFromExternal);
	std::thread t4(CheckForProfileChangeFromInternal);

	t3.join();
	t4.join();

	return 0;
}