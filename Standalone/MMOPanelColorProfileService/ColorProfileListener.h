#pragma once
#include "Profile.h"
#include <winreg.h>

VOID NewStatus(BOOL, BOOL, INT);

VOID ChangeColorProfile(std::wstring);
VOID ChangeColorProfileNightLight(double);

VOID CheckForProfileChangeFromExternal();
VOID CheckForProfileChangeFromInternal();

std::wstring ReadSelectedProfile(HKEY);

VOID ProvisionDefaultProfileData();
VOID ProvisionProfileListData();

VOID EnableNightLight(ULONG value);
VOID DisableNightLight();

int ColorProfileListenerMain();