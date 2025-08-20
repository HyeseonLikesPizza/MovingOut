
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"


class UOverlayWidgetController;
class UTextBlock;
class UProgressBar;

UCLASS()
class MOVINGOUT_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UOverlayWidgetController* InWC);

	
protected:
	virtual void NativeDestruct() override;
	
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TimerText = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ObjectiveText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	UProgressBar* TimeProgressBar = nullptr;

	UPROPERTY()
	UOverlayWidgetController* WC = nullptr;

	UFUNCTION()
	void HandleTimerText(const FText& NewText);

	UFUNCTION()
	void HandleObjective(int32 Remaining, int32 Total);

	UFUNCTION()
	void HandleMedal(EMedal Medal);


	void UpdateTimeProgressBar(int32 ElapsedTime, int32 Total);
	
};
