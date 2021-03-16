#pragma once
#include "Profile.h"

VOID NewStatus(BOOL, BOOL, INT);

VOID NightModeEnabled(INT);
VOID NightModeDisabled();

VOID ChangeColorProfile(std::wstring);
VOID ChangeColorProfileNightLight(double);

VOID CheckForProfileChangeFromExternal();
VOID CheckForProfileChangeFromInternal();

std::wstring ReadSelectedProfile();

VOID ProvisionDefaultProfileData();
VOID ProvisionProfileListData();

VOID EnableNightLight(ULONG value);
VOID DisableNightLight();

int ColorProfileListenerMain();