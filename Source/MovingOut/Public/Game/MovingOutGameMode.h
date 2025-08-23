
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UI/Subsystem/UIManagerSubsystem.h"
#include "MovingOutGameMode.generated.h"

class AMovingOutGameState;
class UURankTimeConfig;

UCLASS(minimalapi)
class AMovingOutGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMovingOutGameMode();

	virtual void BeginPlay() override;

	// 채점 기준
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float GoldSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float SilverSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float BronzeSec = 180.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score|Rules")
	float FailOverSec = 180.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Medal")
	TObjectPtr<UURankTimeConfig> MedalThresholdDA;
	

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



