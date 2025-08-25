
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "EndGameWidget.generated.h"

UCLASS()
class MOVINGOUT_API UEndGameWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* G_Begin;
	
protected:
	virtual void NativeConstruct() override;
};
