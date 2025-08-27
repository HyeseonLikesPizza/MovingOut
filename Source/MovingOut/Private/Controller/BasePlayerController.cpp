
#include "Controller/BasePlayerController.h"
#include "UI/Subsystem/UIManagerSubsystem.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UUIManagerSubsystem* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			UI->ApplyInitialUI();
		}
	}
}
