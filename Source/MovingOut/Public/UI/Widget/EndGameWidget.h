
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "EndGameWidget.generated.h"

UCLASS()
class MOVINGOUT_API UEndGameWidget : public UMovingOutWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
};
