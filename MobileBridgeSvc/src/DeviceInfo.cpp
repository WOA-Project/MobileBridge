#include "DeviceInfo.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <map>

struct cmp_str
{
	bool operator()(char const* a, char const* b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

std::map<const char*, const char*, cmp_str> device_map =
{
	{ "RM-1085", "Lumia 950 XL" },
	{ "RM-1104", "Lumia 950" },
	{ "RM-1105", "Lumia 950" },
	{ "RM-1106", "Lumia 950" },
	{ "RM-1116", "Lumia 950 XL Dual SIM" },
	{ "RM-1118", "Lumia 950 Dual SIM" },
	{ "RX-127", "Lumia Hapanero" },
	{ "RX-130", "Lumia Hapanero" }
};

#define OEM_INFORMATION "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OEMInformation"
#define DEVICE_TARGETING_INFO "SYSTEM\\Platform\\DeviceTargetingInfo"
#define UNIVERSAL_SERIAL_BUS_FUNCTION "SYSTEM\\CurrentControlSet\\Control\\USBFN"

#define SUPPORT_HOURS "Monday through Friday, 5:00 AM - 9:00 PM Pacific Time and Saturday and Sunday, 6:00 AM - 3:00 PM Pacific Time"
#define SUPPORT_PHONE "1-800-Microsoft (642-7676), TTY: 1-800-892-5234"
#define SUPPORT_URL   "http://lumia.linkmicrosoft.com/device/entry/van/nsupport"
#define LOGO          "%%SystemRoot%%\\System32\\OEM\\MMO.bmp"
#define MANUFACTURER  "Microsoft"
#define MODEL(x)	  BuildModelStr(x)

#define PHONE_FIRMWARE_REVISION			   "01078.00053.16236.35005"
#define PHONE_HARDWARE_REVISION			   "1.0.0.0"
#define PHONE_MANUFACTURER MANUFACTURER    "MicrosoftMDG"
#define PHONE_MANUFACTURER_DISPLAY_NAME    MANUFACTURER
#define PHONE_MANUFACTURER_MODEL_NAME(x)   BuildModelStr2(x)
#define PHONE_MOBILE_OPERATOR_DISPLAY_NAME "Operator"
#define PHONE_MOBILE_OPERATOR_NAME		   "000-88"
#define PHONE_MODEL_NAME(x)				   device_map[x]
#define PHONE_OEM_SUPPORT_LINK			   SUPPORT_URL
#define PHONE_RADIO_SOFTWARE_REVISION	   "BO25c43.00024.0001"
#define PHONE_ROM_LANGUAGE				   "0809"
#define PHONE_SOC_VERSION(x)			   GetSOC(x)

#define PRODUCT_STRING(x)                  BuildModelStr3(x)

const char* GetSOC(char* x)
{
	if (std::strcmp(x, "RM-1104") == 0 || std::strcmp(x, "RM-1105") == 0 || std::strcmp(x, "RM-1106") == 0 || std::strcmp(x, "RM-1118") == 0) \
		return "8992";
	return "8994";
}

char str[256] = { 0 };

const char* BuildModelStr(char* x)
{
	memset(str, 0, 256);
	strcat_s(str, MANUFACTURER);
	strcat_s(str, " ");
	strcat_s(str, device_map[x]);
	return str;
}

char str2[256] = { 0 };

const char* BuildModelStr2(char* x)
{
	memset(str2, 0, 256);
	strcat_s(str2, x);
	strcat_s(str2, "-13842");
	return str2;
}

char str3[256] = { 0 };

const char* BuildModelStr3(char* y)
{
	memset(str3, 0, 256);
	strcat_s(str3, device_map[y]);
	strcat_s(str3, y);
	return str3;
}

BOOL DirectoryExists2(const char* dirName) {
	DWORD attribs = ::GetFileAttributes(dirName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL FileExists(const char* dirName) {
	DWORD attribs = ::GetFileAttributes(dirName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return !(attribs & FILE_ATTRIBUTE_DIRECTORY);
}

void MyRegSetValue(const char* key, const char* valueName, const char* value)
{
	LONG status;
	HKEY hKey;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_ALL_ACCESS, &hKey);
	if ((status == ERROR_SUCCESS) && (hKey != NULL))
	{
		status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)value, ((DWORD)strlen(value) + 1) * sizeof(char));
		RegCloseKey(hKey);
	}
	else
	{
		status = RegCreateKeyEx(HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
		if ((status == ERROR_SUCCESS) && (hKey != NULL))
		{
			status = RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)value, ((DWORD)strlen(value) + 1) * sizeof(char));
			RegCloseKey(hKey);
		}
	}
}

HRESULT DeviceInfo::Initialize()
{
	std::fstream ProductFile;
	char ProductCode[512] = { 0 };
	char HardwareRevision[512] = { 0 };
	const char* MMOPath = "C:\\DPP\\MMO\\product.dat";
	const char* NMOPath = "C:\\DPP\\NMO\\product.dat";

	BOOL MMOPresent = DirectoryExists2("C:\\DPP\\MMO");
	BOOL NMOPresent = DirectoryExists2("C:\\DPP\\Nokia");

	if (!MMOPresent && !NMOPresent)
	{
		return ERROR_SUCCESS;
	}

	if ((MMOPresent && !FileExists(MMOPath)) || (NMOPresent && !FileExists(NMOPath)))
	{
		return ERROR_SUCCESS;
	}

	/*
		TYPE:RM-1045
		LPSN:7DAA467AU
		HWID:2003
		MC:0206071
		BTR:059V6V7
		CTR:059W004
	*/

	if (MMOPresent)
	{
		ProductFile.open(MMOPath, std::ios::in);
	}
	else
	{
		ProductFile.open(NMOPath, std::ios::in);
	}

	if (!ProductFile.is_open())
	{
		return ERROR_SUCCESS;
	}

	std::string line;
	while (getline(ProductFile, line))
	{
		if (strncmp(line.c_str(), "TYPE:", 5) == 0)
		{
			strcpy_s(ProductCode, line.c_str() + 5);
		}
		else if(strncmp(line.c_str(), "HWID:", 5) == 0)
		{
			strcpy_s(HardwareRevision, line.c_str() + 5);
		}
	}

	if (strlen(ProductCode) != 0)
	{
		//printf_s("Product Code: %s\n", ProductCode);

		if (device_map.count(ProductCode) <= 0)
		{
			return ERROR_SUCCESS;
		}

		MyRegSetValue(OEM_INFORMATION, "Manufacturer", MANUFACTURER);
		MyRegSetValue(OEM_INFORMATION, "Model", MODEL(ProductCode));
		MyRegSetValue(OEM_INFORMATION, "SupportHours", SUPPORT_HOURS);
		MyRegSetValue(OEM_INFORMATION, "SupportPhone", SUPPORT_PHONE);
		MyRegSetValue(OEM_INFORMATION, "SupportURL", SUPPORT_URL);
		MyRegSetValue(OEM_INFORMATION, "Logo", LOGO);

		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneFirmwareRevision", PHONE_FIRMWARE_REVISION);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneHardwareRevision", PHONE_HARDWARE_REVISION);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneHardwareVariant", ProductCode);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneManufacturer", PHONE_MANUFACTURER);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneManufacturerDisplayName", PHONE_MANUFACTURER_DISPLAY_NAME);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneManufacturerModelName", PHONE_MANUFACTURER_MODEL_NAME(ProductCode));
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneMobileOperatorDisplayName", PHONE_MOBILE_OPERATOR_DISPLAY_NAME);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneMobileOperatorName", PHONE_MOBILE_OPERATOR_NAME);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneModelName", PHONE_MODEL_NAME(ProductCode));
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneOEMSupportLink", PHONE_OEM_SUPPORT_LINK);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneRadioSoftwareRevision", PHONE_RADIO_SOFTWARE_REVISION);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneROMLanguage", PHONE_ROM_LANGUAGE);
		MyRegSetValue(DEVICE_TARGETING_INFO, "PhoneSOCVersion", PHONE_SOC_VERSION(ProductCode));
		MyRegSetValue(UNIVERSAL_SERIAL_BUS_FUNCTION, "ProductString", PRODUCT_STRING(ProductCode));
	}

	if (strlen(HardwareRevision) != 0)
	{
		//printf_s("Hardware Revision: %s\n", HardwareRevision);
	}

	return ERROR_SUCCESS;
}