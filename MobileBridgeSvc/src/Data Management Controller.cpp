#include "Data Management Controller.h"
#include "pch.h"

GUID WWAN_PROFILE_SET_ALL = { 0x31A32D76, 0xE07F, 0x499D, {0x8F, 0xA3, 0xDD, 0xA7, 0x3B, 0xA7, 0x70, 0x57 } };

HRESULT DataManagementController::Initialize(HANDLE g_ServiceStopEvent)
{
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 1;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	int iRet = 0;
	int iterationMax = 5;
	int iteration = 0;

	WCHAR GuidString[40] = { 0 };

	PWWAN_INTERFACE_INFO_LIST pIfList = NULL;

	dwResult = WwanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS)
	{
		return 1;
	}

	while (TRUE)
	{
		dwResult = WwanEnumerateInterfaces(hClient, NULL, &pIfList);
		if (dwResult != ERROR_SUCCESS)
		{
			break;
		}
		else
		{
			iteration++;
			if (iteration > iterationMax)
				break;

			BOOL HasActivated = FALSE;

			for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
				WWAN_INTERFACE_INFO pIfInfo = pIfList->InterfaceInfo[i];
				if (StringFromGUID2(pIfInfo.InterfaceGuid, (LPOLESTR)&GuidString, 39))
				{
					if (pIfInfo.InterfaceStatus.InterfaceState == WwanInterfaceStateAttached)
					{
						WWAN_DATA_ENABLEMENT de = { WWAN_PROFILE_SET_ALL, 1 };
						dwResult = WwanSetInterface(hClient, &pIfInfo.InterfaceGuid, WwanIntfOpcodeDataEnablement, sizeof(WWAN_DATA_ENABLEMENT), &de, NULL, NULL, NULL);
						HasActivated = TRUE;
					}
				}
			}

			if (HasActivated)
				break;
		}

		if (pIfList != NULL) {
			WwanFreeMemory(pIfList);
			pIfList = NULL;
		}

		Sleep(5000);
	}

	if (hClient != NULL)
		WwanCloseHandle(hClient, NULL);

	return ERROR_SUCCESS;
}