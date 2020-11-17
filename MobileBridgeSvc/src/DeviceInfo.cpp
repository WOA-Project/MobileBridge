#include "DeviceInfo.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <map>
#include "Utilities.h"

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
#define PHONE_MANUFACTURER_MODEL_NAME(x)   BuildManufacturerModelNameStr(x)
#define PHONE_MOBILE_OPERATOR_DISPLAY_NAME "Operator"
#define PHONE_MOBILE_OPERATOR_NAME		   "000-88"
#define PHONE_MODEL_NAME(x)				   device_map[x]
#define PHONE_OEM_SUPPORT_LINK			   SUPPORT_URL
#define PHONE_RADIO_SOFTWARE_REVISION	   "BO25c43.00024.0001"
#define PHONE_ROM_LANGUAGE				   "0809"
#define PHONE_SOC_VERSION(x)			   GetSystemOnChipStr(x)

#define PRODUCT_STRING(x)                  BuildProductStr(x)

char ModelStr[256] = { 0 };
char ManufacturerModelNameStr[256] = { 0 };
char ProductStr[256] = { 0 };

const char* GetSystemOnChipStr(char* x)
{
	if (std::strcmp(x, "RM-1104") == 0 || std::strcmp(x, "RM-1105") == 0 || std::strcmp(x, "RM-1106") == 0 || std::strcmp(x, "RM-1118") == 0)
		return "8992";
	return "8994";
}

const char* BuildModelStr(char* x)
{
	memset(ModelStr, 0, 256);
	strcat_s(ModelStr, MANUFACTURER);
	strcat_s(ModelStr, " ");
	strcat_s(ModelStr, device_map[x]);
	sprintf_s(ModelStr, MANUFACTURER " %s", device_map[x]);
	return ModelStr;
}

const char* BuildManufacturerModelNameStr(char* x)
{
	memset(ManufacturerModelNameStr, 0, 256);
	sprintf_s(ManufacturerModelNameStr, "%s-13842", x);
	return ManufacturerModelNameStr;
}

const char* BuildProductStr(char* y)
{
	memset(ProductStr, 0, 256);
	sprintf_s(ProductStr, "%s (%s)", device_map[y], y);
	return ProductStr;
}

HRESULT DeviceInfo::Initialize()
{
	std::fstream ProductFile;
	char ProductCode[512] = { 0 };
	char HardwareRevision[512] = { 0 };
	const char* MMOPath = "C:\\DPP\\MMO\\product.dat";
	const char* NMOPath = "C:\\DPP\\NMO\\product.dat";

	BOOL MMOPresent = Utilities::DoesDirectoryExist("C:\\DPP\\MMO");
	BOOL NMOPresent = Utilities::DoesDirectoryExist("C:\\DPP\\Nokia");

	if (!MMOPresent && !NMOPresent)
	{
		return ERROR_SUCCESS;
	}

	if ((MMOPresent && !Utilities::DoesFileExist(MMOPath)) || (NMOPresent && !Utilities::DoesFileExist(NMOPath)))
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

		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "Manufacturer", MANUFACTURER);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "Model", MODEL(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "SupportHours", SUPPORT_HOURS);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "SupportPhone", SUPPORT_PHONE);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "SupportURL", SUPPORT_URL);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, "Logo", LOGO);

		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneFirmwareRevision", PHONE_FIRMWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneHardwareRevision", PHONE_HARDWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneHardwareVariant", ProductCode);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneManufacturer", PHONE_MANUFACTURER);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneManufacturerDisplayName", PHONE_MANUFACTURER_DISPLAY_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneManufacturerModelName", PHONE_MANUFACTURER_MODEL_NAME(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneMobileOperatorDisplayName", PHONE_MOBILE_OPERATOR_DISPLAY_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneMobileOperatorName", PHONE_MOBILE_OPERATOR_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneModelName", PHONE_MODEL_NAME(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneOEMSupportLink", PHONE_OEM_SUPPORT_LINK);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneRadioSoftwareRevision", PHONE_RADIO_SOFTWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneROMLanguage", PHONE_ROM_LANGUAGE);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, "PhoneSOCVersion", PHONE_SOC_VERSION(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(UNIVERSAL_SERIAL_BUS_FUNCTION, "ProductString", PRODUCT_STRING(ProductCode));
	}

	if (strlen(HardwareRevision) != 0)
	{
		//printf_s("Hardware Revision: %s\n", HardwareRevision);
	}

	return ERROR_SUCCESS;
}