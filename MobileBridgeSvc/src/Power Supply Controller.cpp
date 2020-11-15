#include "Power Supply Controller.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Media.Core.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System::Power;
using namespace Windows::Media::Playback;
using namespace Windows::Media::Core;

#ifdef AUDIO_STARTUP_DELAY
BOOL ShouldRun = FALSE;
#endif

void PlayAlert()
{
#ifdef AUDIO_STARTUP_DELAY
	if (ShouldRun == FALSE)
	{
		return;
	}
#endif

	Uri uri(L"C:\\Windows\\Media\\Alert_charging.wma");
	MediaPlayer player;
	player.SetUriSource(uri);
	player.CommandManager().IsEnabled(false);
	player.Play();
	Sleep(3000);
	player.Close();
}

HRESULT PowerSupplyController::Initialize()
{
	PowerManager::PowerSupplyStatusChanged([](IInspectable const& handler, IInspectable const& args)
		{
			if (PowerManager::PowerSupplyStatus() == PowerSupplyStatus::Adequate)
			{
				PlayAlert();
			}
			else if (PowerManager::PowerSupplyStatus() == PowerSupplyStatus::Inadequate)
			{

			}
		});

#ifdef AUDIO_STARTUP_DELAY
	// Give the audio service some time to initialize.
	Sleep(5000);

	ShouldRun = TRUE;
#endif

	return ERROR_SUCCESS;
}