
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameOverlayWidget.generated.h"


class UOverlayWidgetController;
class UTextBlock;
class UProgressBar;
class UImage;

UCLASS()
class MOVINGOUT_API UInGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UOverlayWidgetController* InWC);

	
protected:
	virtual void NativeDestruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EMedal CurrentState;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TimerText = nullptr;

	UPROPERTY(meta=(BindWidget))
	UImage* TimerImage = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ObjectiveText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	UProgressBar* TimeProgressBar = nullptr;

	UPROPERTY()
	UOverlayWidgetController* WC = nullptr;

	UFUNCTION()
	void HandleTimerText(const FText& NewText);

	UFUNCTION()
	void HandleObjective(int32 Delivered, int32 Total);

	UFUNCTION()
	void HandleMedal(EMedal Medal);

	UFUNCTION()
	void UpdateTimeProgressBar(float ElapsedTime, float Total);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame|Image")
	UTexture2D* BronzeImg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame|Image")
	UTexture2D* SilverImg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InGame|Image")
	UTexture2D* GoldImg;
	
};
