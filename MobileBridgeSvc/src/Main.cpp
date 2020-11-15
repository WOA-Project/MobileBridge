#include <string>
#include "Main.h"
#include "Audio Syncing Controller.h"
#include "Brightness Sensor Controller.h"
#include "Data Management Controller.h"
//#include "Power Supply Controller.h"
#include "DeviceInfo.h"
#include "RILInit.h"
#include <winrt/Windows.Foundation.h>

using namespace winrt;

AudioSyncingController audioSyncingController;
BrightnessSensorController brightnessSensorController;
DataManagementController dataManagementController;
RILInit rilInit;
//PowerSupplyController powerSupplyController;
DeviceInfo deviceInfo;

int _tmain(int argc, TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		//return GetLastError();
		// The application was ran by the user
		// So we run our main procedure

		return ServiceWorkerThread(NULL);
	}

	return 0;
}

#define BUFSIZE MAX_PATH

BOOL DirectoryExists(const char* dirName) {
	DWORD attribs = ::GetFileAttributes(dirName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

int TryToMountDPPIfNeeded()
{
	char Buf[BUFSIZE];     // temporary buffer for volume name

	if (DirectoryExists("C:\\DPP"))
	{
		return -4;
	}

	HANDLE bFlag = FindFirstVolume(
		Buf, // output volume name buffer
		BUFSIZE  // size of volume name buffer
	);

	if (bFlag == NULL)
	{
		return -1;
	}

	if (_wmkdir(L"C:\\DPP") != 0)
	{
		return -2;
	}

	BOOL res = SetVolumeMountPoint(
		"C:\\DPP\\", // mount point
		Buf    // volume to be mounted
	);

	if (res == NULL)
	{
		return -3;
	}

	return 0;
}


VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	DWORD Status = E_FAIL;
	HANDLE hThread;

	g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
	{
		goto EXIT;
	}

	// Tell the service controller we are starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

	/*
	 * Perform tasks neccesary to start the service here
	 */

	 // Create stop event to wait on later.
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL)
	{
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		goto EXIT;
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

	// Start the thread that will perform the main task of the service
	hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Wait until our worker thread exits effectively signaling that the service needs to stop
	WaitForSingleObject(hThread, INFINITE);

	/*
	 * Perform any cleanup tasks
	 */
	CloseHandle(g_ServiceStopEvent);

	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

EXIT:
	return;
}


VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		 * Perform tasks neccesary to stop the service here
		 */

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		// This will signal the worker thread to start shutting down
		SetEvent(g_ServiceStopEvent);

		break;

	default:
		break;
	}
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	init_apartment();

	TryToMountDPPIfNeeded();
	audioSyncingController.Initialize();
	brightnessSensorController.Initialize();
	rilInit.Initialize(g_ServiceStopEvent);
	dataManagementController.Initialize(g_ServiceStopEvent);
	deviceInfo.Initialize();
	//Sleep(4000);
	//powerSupplyController.Initialize();

	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		Sleep(1000);
	}

	audioSyncingController.DeInitialize();

	return ERROR_SUCCESS;
}