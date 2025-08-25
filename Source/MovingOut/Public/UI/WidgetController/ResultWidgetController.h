
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "Type/MedalTypes.h"
#include "ResultWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnResultData, bool, bVictory, FText, ClearTime, int32, ItemsDelivered, int32, ItemsTotal, FMedalThresholds, MedalThresholds);

UCLASS()
class MOVINGOUT_API UResultWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Result")
	void PushDataToWidget();
	
	UPROPERTY(BlueprintAssignable)
	FOnResultData Pushed;

private:
	FGameResultData Cached;
	
};
