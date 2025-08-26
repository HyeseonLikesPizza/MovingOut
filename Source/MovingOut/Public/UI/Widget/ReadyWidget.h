
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "ReadyWidget.generated.h"


DECLARE_DELEGATE(FOnGameStart);

UCLASS()
class MOVINGOUT_API UReadyWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* R_Begin;

	FOnGameStart OnGameStart;

	virtual void NativeConstruct() override;
};
