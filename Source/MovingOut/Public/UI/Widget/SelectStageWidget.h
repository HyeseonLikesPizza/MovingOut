
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/MovingOutWidget.h"
#include "SelectStageWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestStageInfo);

UCLASS()
class MOVINGOUT_API USelectStageWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnRequestStageInfo OnRequestStageInfo;

protected:
	virtual void NativeConstruct() override;
};
