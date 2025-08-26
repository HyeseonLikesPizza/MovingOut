
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWinWidget.generated.h"

class UResultWidgetController;
class UTextBlock;
struct FMedalThresholds;
class UImage;

UCLASS()
class MOVINGOUT_API UEndGameWinWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UResultWidgetController* InWC);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// WBP 변수
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* G_Begin;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_ClearTime;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_BestClearTime;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_GoldTimeThreshold;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_SilverTimeThreshold;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_BronzeTimeThreshold;

	
	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_Goal1;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_Goal2;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_Goal3;

	UPROPERTY(meta=(BindWidget), Transient)
	UImage* Img_Goal1;

	UPROPERTY(meta=(BindWidget), Transient)
	UImage* Img_Goal2;

	UPROPERTY(meta=(BindWidget), Transient)
	UImage* Img_Goal3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Result|Icon")
	UTexture2D* SuccessIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Result|Icon")
	UTexture2D* FailIcon;


private:

	UFUNCTION()
	void HandleResultData(FText ClearTimeText,  const TArray<FText>& MedalThresholds, const TArray<FAdditionalGoalData>& AdditionalGoal);
	void SetClearTimeText(FText ClearTimeText);
	void SetMedalThresholds(const TArray<FText>& MedalThresholds);
	void SetAdditionalGoals(const TArray<FAdditionalGoalData>& InGoals);

	UPROPERTY()
	UResultWidgetController* WC;
};
