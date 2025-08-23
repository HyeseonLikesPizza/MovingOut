
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "TitleScreenWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRequested);

UCLASS()
class MOVINGOUT_API UTitleScreenWidget : public UMovingOutWidget
{
	GENERATED_BODY()


public:
	UTitleScreenWidget(const FObjectInitializer& OI);
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnStartRequested OnStartRequested;

protected:

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* T_Begin;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* T_End;
	
	
};
