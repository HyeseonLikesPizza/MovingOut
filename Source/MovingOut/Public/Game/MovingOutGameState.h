
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MovingOutGameState.generated.h"


UENUM(BlueprintType)
enum class EMedal : uint8
{
	None,
	Gold,
	Silver,
	Bronze,
	Fail
};


UCLASS()
class MOVINGOUT_API AMovingOutGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// 타이머 상태
	UPROPERTY(Replicated)
	float AccumulatedSeconds = 0.f;

	UPROPERTY(Replicated)
	float RunningStartTime = 0.f;

	UPROPERTY(Replicated)
	bool bTimerRunning = false;

	

	// 종료 고정값, 플래그

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bPlayStopped = true;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float FinalElapsedSeconds = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	EMedal ResultMedal = EMedal::None;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bVictory = false;


	// 시작 (서버에서 호출)
	UFUNCTION(BlueprintCallable)
	void StartMatchTimer();

	// 일시정지/재개 (서버에서 호출)
	UFUNCTION(BlueprintCallable)
	void PauseMatchTimer();

	UFUNCTION(BlueprintCallable)
	void ResumeMatchTimer();

	UFUNCTION(BlueprintCallable)
	void StopMatchTimer();

	//  경과 시간(초) 얻기
	UFUNCTION(BlueprintPure)
	float GetElapsedTimeSeconds() const;

	// 메달 채점
	UFUNCTION(BlueprintPure)
	EMedal EvaluateMedal(float GoldSec, float SilverSSec, float BronzeSec, float FailOverSec) const;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	
};
