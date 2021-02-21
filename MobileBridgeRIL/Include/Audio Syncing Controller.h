#ifndef AUDIOSYNCCONTROLLER_H_
#define AUDIOSYNCCONTROLLER_H_

#include <atlcomcli.h>
#include <atlcoll.h>
#include <Windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <string.h>
#include "mmdeviceapi.h"
#include "devicetopology.h"

class AudioSyncingController
{
public:
	HRESULT Initialize();
	HRESULT DeInitialize();
};

#endif