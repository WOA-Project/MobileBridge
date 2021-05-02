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

std::map<const char*, const wchar_t*, cmp_str> device_map =
{
    { "RM-801", L"Lumia 800" },
    { "RM-802", L"Lumia 800" },
    { "RM-803", L"Lumia 710" },
    { "RM-808", L"Lumia 900" },
    { "RM-809", L"Lumia 710" },
    { "RM-819", L"Lumia 800" },
    { "RM-820", L"Lumia 920" },
    { "RM-821", L"Lumia 920" },
    { "RM-822", L"Lumia 920" },
    { "RM-823", L"Lumia 900" },
    { "RM-824", L"Lumia 820" },
    { "RM-825", L"Lumia 820" },
    { "RM-826", L"Lumia 820" },
    { "RM-835", L"Lumia 610" },
    { "RM-836", L"Lumia 610" },
    { "RM-845", L"Lumia 822" },
    { "RM-846", L"Lumia 620" },
    { "RM-849", L"Lumia 610" },
    { "RM-860", L"Lumia 928" },
    { "RM-867", L"Lumia 920T" },
    { "RM-875", L"Lumia 1020" },
    { "RM-876", L"Lumia 1020" },
    { "RM-877", L"Lumia 1020" },
    { "RM-878", L"Lumia 810" },
    { "RM-885", L"Lumia 720" },
    { "RM-887", L"Lumia 720T" },
    { "RM-889", L"Lumia 510" },
    { "RM-892", L"Lumia 925" },
    { "RM-893", L"Lumia 925" },
    { "RM-898", L"Lumia 510" },
    { "RM-910", L"Lumia 925" },
    { "RM-913", L"Lumia 520" },
    { "RM-914", L"Lumia 520" },
    { "RM-915", L"Lumia 520" },
    { "RM-917", L"Lumia 521" },
    { "RM-923", L"Lumia 505" },
    { "RM-927", L"Lumia 929 Icon" },
    { "RM-937", L"Lumia 1520" },
    { "RM-938", L"Lumia 1520" },
    { "RM-939", L"Lumia 1520" },
    { "RM-940", L"Lumia 1520" },
    { "RM-941", L"Lumia 625" },
    { "RM-942", L"Lumia 625" },
    { "RM-943", L"Lumia 625" },
    { "RM-955", L"Lumia 925T" },
    { "RM-964", L"GoldFinger (Codename)" },
    { "RM-974", L"Lumia 635" },
    { "RM-975", L"Lumia 635" },
    { "RM-976", L"Lumia 630" },
    { "RM-977", L"Lumia 630" },
    { "RM-978", L"Lumia 630 Dual SIM" },
    { "RM-979", L"Lumia 630 Dual SIM" },
    { "RM-983", L"Lumia 830" },
    { "RM-984", L"Lumia 830" },
    { "RM-985", L"Lumia 830" },
    { "RM-994", L"Lumia 1320" },
    { "RM-995", L"Lumia 1320" },
    { "RM-996", L"Lumia 1320" },
    { "RM-997", L"Lumia 526" },
    { "RM-998", L"Lumia 525" },
    { "RM-1001", L"McLaren (Codename) (Version 1)" },
    { "RM-1002", L"McLaren (Codename) (Version 1)" },
    { "RM-1003", L"McLaren (Codename) (Version 1)" },
    { "RM-1004", L"McLaren (Codename) (Version 1)" },
    { "RM-1010", L"Lumia 638" },
    { "RM-1017", L"Lumia 530" },
    { "RM-1018", L"Lumia 530" },
    { "RM-1019", L"Lumia 530 Dual SIM" },
    { "RM-1020", L"Lumia 530 Dual SIM" },
    { "RM-1027", L"Lumia 636" },
    { "RM-1031", L"Lumia 532 Dual SIM" },
    { "RM-1032", L"Lumia 532" },
    { "RM-1034", L"Lumia 532" },
    { "RM-1038", L"Lumia 735 4G" },
    { "RM-1039", L"Lumia 735" },
    { "RM-1040", L"Lumia 730 Dual SIM" },
    { "RM-1041", L"Lumia 735" },
    { "RM-1045", L"Lumia 930" },
    { "RM-1049", L"Lumia 830" },
    { "RM-1051", L"McLaren (Codename) (Version 2)" },
    { "RM-1052", L"McLaren (Codename) (Version 2)" },
    { "RM-1053", L"McLaren (Codename) (Version 2)" },
    { "RM-1054", L"McLaren (Codename) (Version 2)" },
    { "RM-1062", L"Lumia 640 XL LTE" },
    { "RM-1063", L"Lumia 640 XL LTE" },
    { "RM-1064", L"Lumia 640 XL LTE" },
    { "RM-1065", L"Lumia 640 XL Dual SIM" },
    { "RM-1066", L"Lumia 640 XL" },
    { "RM-1067", L"Lumia 640 XL Dual SIM" },
    { "RM-1068", L"Lumia 435" },
    { "RM-1069", L"Lumia 435 Dual SIM" },
    { "RM-1070", L"Lumia 435" },
    { "RM-1071", L"Lumia 435" },
    { "RM-1072", L"Lumia 640 LTE" },
    { "RM-1073", L"Lumia 640 LTE" },
    { "RM-1074", L"Lumia 640 LTE" },
    { "RM-1075", L"Lumia 640 Dual SIM" },
    { "RM-1077", L"Lumia 640 Dual SIM" },
    { "RM-1078", L"Lumia 735" },
    { "RM-1085", L"Lumia 950 XL" },
    { "RM-1087", L"Lumia 930" },
    { "RM-1089", L"Lumia 535" },
    { "RM-1090", L"Lumia 535 Dual SIM" },
    { "RM-1091", L"Lumia 535" },
    { "RM-1092", L"Lumia 535 Dual SIM" },
    { "RM-1096", L"Lumia 640 XL LTE Dual SIM" },
    { "RM-1099", L"Lumia 430 Dual SIM" },
    { "RM-1104", L"Lumia 950" },
    { "RM-1105", L"Lumia 950" },
    { "RM-1106", L"Lumia 950" },
    { "RM-1109", L"Lumia 640" },
    { "RM-1113", L"Lumia 640 LTE Dual SIM" },
    { "RM-1114", L"Lumia 435 Dual SIM" },
    { "RM-1115", L"Lumia 532 Dual SIM" },
    { "RM-1116", L"Lumia 950 XL Dual SIM" },
    { "RM-1118", L"Lumia 950 Dual SIM" },
    { "RM-1127", L"Lumia 550" },
    { "RM-1128", L"Lumia 550" },
    { "RM-1140", L"Lumia 540 Dual SIM" },
    { "RM-1141", L"Lumia 540 Dual SIM" },
    { "RM-1142", L"Guilin (Codename)" },
    { "RM-1147", L"Guilin (Codename) Dual SIM" },
    { "RM-1150", L"Lumia 650" },
    { "RM-1152", L"Lumia 650" },
    { "RM-1153", L"Lumia 650" },
    { "RM-1154", L"Lumia 650 Dual SIM" },
    { "RM-1155", L"Honjo (Codename)" },
    { "RM-1156", L"Honjo (Codename)" },
    { "RM-1157", L"Honjo (Codename)" },
    { "RM-1158", L"Honjo (Codename) Dual SIM" },
    { "RM-1160", L"Northstar (Codename)" },
    { "RM-1161", L"Northstar (Codename)" },
    { "RM-1162", L"Northstar (Codename)" },
    { "RX-100", L"Alpha (Codename)" },
    { "RX-112", L"Midas (Codename)" },
    { "RX-127", L"Hapanero (Codename) (Version 1)" },
    { "RX-130", L"Hapanero (Codename) (Version 2)" }
};

#define OEM_INFORMATION L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OEMInformation"
#define DEVICE_TARGETING_INFO L"SYSTEM\\Platform\\DeviceTargetingInfo"
#define UNIVERSAL_SERIAL_BUS_FUNCTION L"SYSTEM\\CurrentControlSet\\Control\\USBFN"

#define SUPPORT_HOURS L"Monday through Friday, 5:00 AM - 9:00 PM Pacific Time and Saturday and Sunday, 6:00 AM - 3:00 PM Pacific Time"
#define SUPPORT_PHONE L"1-800-Microsoft (642-7676), TTY: 1-800-892-5234"
#define SUPPORT_URL   L"https://support.microsoft.com"
#define LOGO          L"%%SystemRoot%%\\System32\\OEM\\MMO.bmp"
#define MANUFACTURER  L"Microsoft"
#define MODEL(x)	  BuildModelStr(x)

#define PHONE_FIRMWARE_REVISION			   L"01078.00053.16236.35005"
#define PHONE_HARDWARE_REVISION			   L"1.0.0.0"
#define PHONE_MANUFACTURER                 L"MicrosoftMDG"
#define PHONE_MANUFACTURER_DISPLAY_NAME    MANUFACTURER
#define PHONE_MANUFACTURER_MODEL_NAME(x)   BuildManufacturerModelNameStr(x)
#define PHONE_MOBILE_OPERATOR_DISPLAY_NAME L"Operator"
#define PHONE_MOBILE_OPERATOR_NAME		   L"000-88"
#define PHONE_MODEL_NAME(x)				   device_map[x]
#define PHONE_OEM_SUPPORT_LINK			   SUPPORT_URL
#define PHONE_RADIO_SOFTWARE_REVISION	   L"BO25c43.00024.0001"
#define PHONE_ROM_LANGUAGE				   L"0809"
#define PHONE_SOC_VERSION(x)			   GetSystemOnChipStr(x)

#define PRODUCT_STRING(x)                  BuildProductStr(x)

wchar_t ModelStr[256] = { 0 };
wchar_t ManufacturerModelNameStr[256] = { 0 };
wchar_t ProductStr[256] = { 0 };

const wchar_t* GetWCHAR(const char* c)
{
    size_t cSize = strlen(c) + 1;
    size_t convertedChars;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs_s(&convertedChars, wc, cSize, c, cSize);

    return wc;
}

const wchar_t* GetSystemOnChipStr(char* x)
{
	if (std::strcmp(x, "RM-1104") == 0 || std::strcmp(x, "RM-1105") == 0 || std::strcmp(x, "RM-1106") == 0 || std::strcmp(x, "RM-1118") == 0)
		return L"8992";
	if (std::strcmp(x, "RM-1045") == 0 || std::strcmp(x, "RM-1087") == 0)
		return L"8974";
	return L"8994";
}

const wchar_t* BuildModelStr(char* x)
{
	memset(ModelStr, 0, 256 * sizeof(wchar_t));
    swprintf_s(ModelStr, MANUFACTURER L" %s", device_map[x]);
	return ModelStr;
}

const wchar_t* BuildManufacturerModelNameStr(char* x)
{
	memset(ManufacturerModelNameStr, 0, 256 * sizeof(wchar_t));
    const wchar_t* code = GetWCHAR(x);
    swprintf_s(ManufacturerModelNameStr, L"%s_13842", code);
	return ManufacturerModelNameStr;
}

const wchar_t* BuildProductStr(char* y)
{
	memset(ProductStr, 0, 256 * sizeof(wchar_t));
    const wchar_t* code = GetWCHAR(y);
    swprintf_s(ProductStr, L"%s (%s)", device_map[y], code);
	return ProductStr;
}

HRESULT DeviceInfo::Initialize()
{
	std::fstream ProductFile;
	char ProductCode[512] = { 0 };
	char HardwareRevision[512] = { 0 };
	const wchar_t* MMOPath = L"C:\\DPP\\MMO\\product.dat";
	const wchar_t* NMOPath = L"C:\\DPP\\Nokia\\product.dat";

	BOOL MMOPresent = Utilities::DoesDirectoryExist(L"C:\\DPP\\MMO");
	BOOL NMOPresent = Utilities::DoesDirectoryExist(L"C:\\DPP\\Nokia");

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

		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"Manufacturer", MANUFACTURER);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"Model", MODEL(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"SupportHours", SUPPORT_HOURS);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"SupportPhone", SUPPORT_PHONE);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"SupportURL", SUPPORT_URL);
		Utilities::RegSetValueAndCreateKeyIfNeeded(OEM_INFORMATION, L"Logo", LOGO);

		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneFirmwareRevision", PHONE_FIRMWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneHardwareRevision", PHONE_HARDWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneHardwareVariant", GetWCHAR(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneManufacturer", PHONE_MANUFACTURER);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneManufacturerDisplayName", PHONE_MANUFACTURER_DISPLAY_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneManufacturerModelName", PHONE_MANUFACTURER_MODEL_NAME(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneMobileOperatorDisplayName", PHONE_MOBILE_OPERATOR_DISPLAY_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneMobileOperatorName", PHONE_MOBILE_OPERATOR_NAME);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneModelName", PHONE_MODEL_NAME(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneOEMSupportLink", PHONE_OEM_SUPPORT_LINK);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneRadioSoftwareRevision", PHONE_RADIO_SOFTWARE_REVISION);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneROMLanguage", PHONE_ROM_LANGUAGE);
		Utilities::RegSetValueAndCreateKeyIfNeeded(DEVICE_TARGETING_INFO, L"PhoneSOCVersion", PHONE_SOC_VERSION(ProductCode));
		Utilities::RegSetValueAndCreateKeyIfNeeded(UNIVERSAL_SERIAL_BUS_FUNCTION, L"ProductString", PRODUCT_STRING(ProductCode));
	}

	if (strlen(HardwareRevision) != 0)
	{
		//printf_s("Hardware Revision: %s\n", HardwareRevision);
	}

	return ERROR_SUCCESS;
}