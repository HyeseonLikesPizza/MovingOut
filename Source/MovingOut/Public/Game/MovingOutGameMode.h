
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MovingOutGameMode.generated.h"

class AMovingOutGameState;

UCLASS(minimalapi)
class AMovingOutGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMovingOutGameMode();

	// 채점 기준
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float GoldSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float SilverSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float BronzeSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float FailOverSec = 180.f;
	

	// 흐름 제어
	UFUNCTION(BlueprintCallable, Category = "Score|Flow")
	void StartMatchFlow();

	UFUNCTION(BlueprintCallable, Category = "Score|Flow")
	void PauseMatchFlow();

	UFUNCTION(BlueprintCallable, Category = "Score|Flow")
	void ResumeMatchFlow();

	UFUNCTION(BlueprintCallable, Category = "Score|Flow")
	void FinishMatchFlow();


protected:
	virtual void StartPlay() override;


private:
	FTimerHandle TimeoutHandle;


	// 유틸
	AMovingOutGameState* GetMGameState() const;
	void HandleTimeout();
	void UpdateTimeoutTimer();
	
	
};



