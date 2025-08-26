
#include "UI/WidgetController/ResultWidgetController.h"
#include "UI/Subsystem/UIManagerSubsystem.h"
#include "Type/MedalTypes.h"

void UResultWidgetController::PushDataToWidget()
{
	if (!PC.Get()) return;
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (auto* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			Cached = UI->GetLastResult();
			bool victory = Cached.bVictory;
			int32 Delivered = Cached.ItemDelivered;
			int32 Total = Cached.ItemTotal;
			FMedalThresholds T = Cached.Thresholds;
			Pushed.Broadcast(
				Cached.bVictory,
				FormatElapsed(Cached.ClearTimeSeconds),
				Cached.ItemDelivered,
				Cached.ItemTotal,
				Cached.Thresholds);
		}
	}
}
