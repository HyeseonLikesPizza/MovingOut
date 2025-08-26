
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "Type/MedalTypes.h"
#include "ResultWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnResultFail, FText, ClearTime, int32, ItemsDelivered, int32, ItemsTotal, const TArray<FText>&, MedalThresholds, const TArray<FAdditionalGoalData>&, AdditionalGoals);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnResultWin, FText, ClearTime, const TArray<FText>&, FMedalThresholds, const TArray<FAdditionalGoalData>&, AdditionalGoals);

UCLASS()
class MOVINGOUT_API UResultWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Result")
	void PushDataToWidget();
	
	UPROPERTY(BlueprintAssignable)
	FOnResultFail OnResultFail;

	UPROPERTY(BlueprintAssignable)
	FOnResultWin OnResultWin;

	TArray<FText> FormatMedalThresholdElapsed(FMedalThresholds thresholds);
	FText FormatSeconds(float InSeconds);

private:
	FGameResultData Cached;
	
};
