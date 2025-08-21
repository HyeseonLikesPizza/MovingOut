
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BaseWidgetController.generated.h"


class AMovingOutGameMode;
class AMovingOutGameState;
class APlayerState;
class APlayerController;


UCLASS()
class MOVINGOUT_API UBaseWidgetController : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	virtual void InitializeController(APlayerController* InPC, AMovingOutGameState* InGS, AMovingOutGameMode* InGM);

	UFUNCTION(BlueprintCallable)
	virtual void Bind();

	UFUNCTION(BlueprintCallable)
	virtual void Unbind();

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerController> PC;
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<APlayerState> PS;
	
	UPROPERTY(BlueprintReadOnly)
	AMovingOutGameState* GS;
	
	UPROPERTY(BlueprintReadOnly)
	AMovingOutGameMode* GM;

	// 편의 함수
	FText FormatElapsed(float Seconds) const;
};
