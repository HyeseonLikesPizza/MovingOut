
#pragma once

#include "CoreMinimal.h"
#include "BaseWidgetController.h"
#include "Game/MovingOutGameState.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerTextChanged, const FText&, NewText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveChanged, int32, remaining, int32, Total);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMedalChanged, EMedal, Medal);

UCLASS()
class MOVINGOUT_API UOverlayWidgetController : public UBaseWidgetController
{
	GENERATED_BODY()

public:
	virtual void Bind() override;
	virtual void Unbind() override;

	UPROPERTY(BlueprintAssignable)
	FOnTimerTextChanged OnTimerTextChanged;

	UPROPERTY(BlueprintAssignable)
	FOnObjectiveChanged OnObjectiveChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnMedalChanged OnMedalChanged;

private:
	FTimerHandle TimerHandle_UIRefresh;
	bool bLastPlayStopped = false;
	EMedal LastMedal      = EMedal::None;

	void TickUI(); // 타이머 텍스트/메달 변화 감지

	UFUNCTION()
	void HandleItemsProgress(int32 Remaining, int32 Total); // OnRep 이벤트 핸들러
	


	
};
