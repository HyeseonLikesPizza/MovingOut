
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MovingOut/Public/Type/MedalTypes.h"
#include "MovingOutGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemsProgress, int32, Remaining, int32, Total);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStopped);

UCLASS()
class MOVINGOUT_API AMovingOutGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AMovingOutGameState();

	virtual void Tick(float DeltaTime) override;

	// 타이머 상태
	UPROPERTY(Replicated)
	float AccumulatedSeconds = 0.f;

	UPROPERTY(Replicated)
	float RunningStartTime = 0.f;

	UPROPERTY(Replicated)
	bool bTimerRunning = false;

	// 시간 등급 정보
	UPROPERTY()
	FMedalThresholds MedalThresholds;
	
	// 종료 고정값, 플래그

	UPROPERTY(ReplicatedUsing = OnRep_PlayStopped)
	bool bPlayStopped = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float FinalElapsedSeconds = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	EMedal ResultMedal = EMedal::None;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bVictory = false;
	
	void SetItemsDelivered(int32 InDelivered);
	int32 GetItemsDelivered() const;
	void SetPlacedProps(int32 InPlacedProps);
	int32 GetPlacedProps() const;
	void InitializePlacedPropsCnt();
	
	UFUNCTION(BlueprintPure)
	int32 GetItemsRemaining() const { return FMath::Max(0, PlacedPropsCnt - ItemsDelivered); }

	
	// Delegate

	UFUNCTION()
	void OnRep_PlayStopped();

	
	UPROPERTY(BlueprintAssignable)
	FOnItemsProgress OnItemsProgress;

	UPROPERTY(BlueprintAssignable)
	FOnMatchStopped OnMatchStopped;
	

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

private:
	
	UPROPERTY(Replicated)
	int32 ItemsDelivered = 0;

	UPROPERTY(Replicated)
	int32 PlacedPropsCnt = -1;


};
