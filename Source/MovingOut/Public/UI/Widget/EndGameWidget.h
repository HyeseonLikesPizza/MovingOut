
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Type/MedalTypes.h"
#include "EndGameWidget.generated.h"

class UResultWidgetController;
class UTextBlock;
struct FMedalThresholds;

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
	UTextBlock* Text_ClearTime;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_ItemsDelivered;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_ItemsTotal;

	/*
	UPROPERTY(meta=(BindWidget), Transient)
	UImage* Image_Victory;
	

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_GoldTimeThreshold;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_SilverTimeThreshold;

	UPROPERTY(meta=(BindWidget), Transient)
	UTextBlock* Text_BronzeTimeThreshold;
	*/

private:

	UFUNCTION()
	void HandleResultData(bool bVictory, FText ClearTimeText, int32 ItemsDelivered, int32 ItemsTotal, FMedalThresholds MedalThresholds);

	void SetClearTimeText(FText ClearTimeText);
	void SetVictoryImage(bool bVictory);
	void SetItemsDeliveredTotal(int32 ItemsDelivered, int32 ItemsTotal);
	void SetMedalThresholds(FMedalThresholds MedalThresholds);

	UPROPERTY()
	UResultWidgetController* WC;
};
