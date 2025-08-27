
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/MovingOutWidget.h"
#include "SelectStageWidget.generated.h"

UCLASS()
class MOVINGOUT_API USelectStageWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:

	

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* C_Begin;

protected:
	virtual void NativeConstruct() override;
};
