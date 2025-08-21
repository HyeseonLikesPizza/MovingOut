
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovingOutWidget.generated.h"

UCLASS()
class MOVINGOUT_API UMovingOutWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
