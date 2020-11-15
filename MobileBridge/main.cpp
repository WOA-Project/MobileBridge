#include "pch.h"
#include "HapticsNotifications.h"
#include "AutoRotate.h"
#include "ColorProfileListener.h"

int main()
{
	init_apartment();

	std::thread tHaptics(HapticsNotificationsMain);
	std::thread tAutoRotate(AutoRotateMain);
	ColorProfileListenerMain();

	tHaptics.join();
	tAutoRotate.join();
}
