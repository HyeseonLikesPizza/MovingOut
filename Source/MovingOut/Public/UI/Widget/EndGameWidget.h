
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MedalTypes.h"
#include "EndGameWidget.generated.h"

class UResultWidgetController;
class UTextBlock;
struct FMedalThresholds;
class UImage;

UCLASS()
class MOVINGOUT_API UEndGameWidget : public UUserWidget
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
	UTextBlock* Text_ItemsDelivered;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_ItemsTotal;

	/*
	UPROPERTY(meta=(BindWidget), Transient)
	UImage* Image_Victory;
	*/
	
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
	void HandleResultData(FText ClearTimeText, int32 ItemsDelivered, int32 ItemsTotal, const TArray<FText>& MedalThresholds, const TArray<FAdditionalGoalData>& AdditionalGoal);
	
	void SetItemsDeliveredTotal(int32 ItemsDelivered, int32 ItemsTotal);
	void SetMedalThresholds(const TArray<FText>& MedalThresholds);
	void SetAdditionalGoals(const TArray<FAdditionalGoalData>& InGoals);

	UPROPERTY()
	UResultWidgetController* WC;
};
