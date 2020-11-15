#pragma once
#include <iostream>
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Security.Credentials.h>
#include <winrt/Windows.Devices.Haptics.h>
#include <winrt/Windows.Devices.Sensors.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Internal.Storage.Cloud.h>


using namespace winrt;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Devices::Haptics;
using namespace Windows::Devices::Sensors;
using namespace Windows::Internal::Storage::Cloud;

const wchar_t* blueLightState = L"windows.data.bluelightreduction.bluelightreductionstate";
const wchar_t* blueLightSettings = L"windows.data.bluelightreduction.settings";