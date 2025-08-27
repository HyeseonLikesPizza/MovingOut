
#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "TruckPlayerController.generated.h"

UCLASS()
class MOVINGOUT_API ATruckPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
