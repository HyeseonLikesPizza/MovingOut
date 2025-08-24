
#include "Game/MovingOutGameState.h"

#include "Async/IAsyncTask.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Props/CountProps.h"

void AMovingOutGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps );

	DOREPLIFETIME(AMovingOutGameState, AccumulatedSeconds);
	DOREPLIFETIME(AMovingOutGameState, RunningStartTime);
	DOREPLIFETIME(AMovingOutGameState, bTimerRunning);
	DOREPLIFETIME(AMovingOutGameState, bPlayStopped);
	DOREPLIFETIME(AMovingOutGameState, FinalElapsedSeconds);
	DOREPLIFETIME(AMovingOutGameState, ItemsDelivered);
	DOREPLIFETIME(AMovingOutGameState, PlacedPropsCnt);
}


AMovingOutGameState::AMovingOutGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMovingOutGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPlayStopped)
	{
		// 경과 시간이 게임오버 시간을 초과했는지 체크
		if (GetElapsedTimeSeconds() > MedalThresholds.FailOverSeconds)
		{
			StopMatchTimer();
		}
	}
}

void AMovingOutGameState::SetItemsDelivered(int32 InDelivered)
{
	ItemsDelivered = InDelivered;
	OnItemsProgress.Broadcast(ItemsDelivered, PlacedPropsCnt);
	if (ItemsDelivered >= PlacedPropsCnt)
	{
		StopMatchTimer();
	}
}

int32 AMovingOutGameState::GetItemsDelivered() const
{
	return ItemsDelivered;
}

void AMovingOutGameState::SetPlacedProps(int32 InPlacedProps)
{
	PlacedPropsCnt = InPlacedProps;
	OnItemsProgress.Broadcast(ItemsDelivered, PlacedPropsCnt);
}

int32 AMovingOutGameState::GetPlacedProps() const
{
	return PlacedPropsCnt;
}

void AMovingOutGameState::InitializePlacedPropsCnt()
{
	TArray<AActor*> Arr;
	UGameplayStatics::GetAllActorsOfClass(this, ACountProps::StaticClass(), Arr);
	SetPlacedProps(Arr.Num());
}

void AMovingOutGameState::OnRep_PlayStopped()
{
	// 클라에서 bPlayStopped true로 바뀐 시점에 동기화된 결과를 알림
	
	if (bPlayStopped)
	{
		OnMatchStopped.Broadcast();
	}
}

void AMovingOutGameState::StartMatchTimer()
{
	if (!HasAuthority()) return;

	const float Now = GetServerWorldTimeSeconds();
	
	AccumulatedSeconds = 0.f;
	RunningStartTime = Now;
	bTimerRunning = true;
	bPlayStopped = false;
	FinalElapsedSeconds = 0.f;
	ResultMedal = EMedal::None;
	bVictory = false;
	
}

void AMovingOutGameState::PauseMatchTimer()
{
	if (!HasAuthority() || !bTimerRunning) return;

	const float Now = GetServerWorldTimeSeconds();
	AccumulatedSeconds += (Now - RunningStartTime);
	bTimerRunning = false;
	
}

void AMovingOutGameState::ResumeMatchTimer()
{
	if (!HasAuthority() || bTimerRunning) return;
	RunningStartTime = GetServerWorldTimeSeconds();
	bTimerRunning = true;
}

void AMovingOutGameState::StopMatchTimer()
{
	if (!HasAuthority() || bPlayStopped)
		return;

	const float Now = GetServerWorldTimeSeconds();
	const float Elapsed = AccumulatedSeconds + (bTimerRunning ? (Now - RunningStartTime) : 0.f);

	FinalElapsedSeconds = FMath::Max(0.f, Elapsed);
	AccumulatedSeconds  = FinalElapsedSeconds; // 어디서 읽어도 동일
	bTimerRunning = false;
	bPlayStopped  = true;
	
	// 결과 등급 확정 (서버에서 결정)
	const float FailOver = MedalThresholds.FailOverSeconds;
	ResultMedal = EvaluateMedal(MedalThresholds.GoldWithinSeconds, MedalThresholds.SilverWithinSeconds,
		MedalThresholds.BronzeWithinSeconds, MedalThresholds.FailOverSeconds);


	// bVictory 결정
	bVictory = (ItemsDelivered >= PlacedPropsCnt) && (FinalElapsedSeconds <= FailOver);


	// 서버에서 브로드캐스트
	OnMatchStopped.Broadcast();
}

float AMovingOutGameState::GetElapsedTimeSeconds() const
{
	if (bPlayStopped)
		return FinalElapsedSeconds;
	
	const float Now = GetServerWorldTimeSeconds();
	return AccumulatedSeconds + (bTimerRunning ? (Now-RunningStartTime) : 0.f);
	
}

EMedal AMovingOutGameState::EvaluateMedal(float GoldSec, float SilverSec, float BronzeSec, float FailOverSec) const
{
	const float S = GetElapsedTimeSeconds();

	if (S<=GoldSec) return EMedal::Gold;
	if (S<=SilverSec) return EMedal::Silver;
	if (S<=BronzeSec) return EMedal::Bronze;
	if (S>FailOverSec) return EMedal::Fail;

	return EMedal::None;
}

