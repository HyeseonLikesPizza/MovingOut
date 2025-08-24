
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "TitleScreenWidget.h"
#include "IntroWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestSelectLevel);

UCLASS()
class MOVINGOUT_API UIntroWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:

	FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRequestSelectLevel OnRequestSelectLevel;

protected:
	virtual void NativeConstruct() override;
	
};
