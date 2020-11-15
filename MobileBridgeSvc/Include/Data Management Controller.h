#pragma once
#include <Windows.h>

class DataManagementController
{
public:
	HRESULT Initialize(HANDLE g_ServiceStopEvent);
};

