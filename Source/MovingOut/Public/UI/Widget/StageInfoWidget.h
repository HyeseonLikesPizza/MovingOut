
#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/MovingOutWidget.h"
#include "StageInfoWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestGameStart);

UCLASS()
class MOVINGOUT_API UStageInfoWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	FOnRequestGameStart OnRequestGameStart;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	virtual void NativeConstruct() override;
	
	
};
