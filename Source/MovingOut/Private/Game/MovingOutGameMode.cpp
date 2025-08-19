
#include "Game/MovingOutGameMode.h"
#include "Controller/MovingOutPlayerController.h"
#include "Game/MovingOutGameState.h"
#include "UObject/ConstructorHelpers.h"

AMovingOutGameMode::AMovingOutGameMode()
{
	PlayerControllerClass = AMovingOutPlayerController::StaticClass();

	/*
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
	*/
}

void AMovingOutGameMode::StartPlay()
{
	Super::StartPlay();
}

void AMovingOutGameMode::StartMatchFlow()
{
	if (!HasAuthority()) return;

	if (auto* G = GetMGameState())
	{
		// 시간 측정 시작
		G->StartMatchTimer();

		// 타임아웃 타이머 무장
		GetWorldTimerManager().ClearTimer(TimeoutHandle);
		GetWorldTimerManager().SetTimer(TimeoutHandle, this, &AMovingOutGameMode::HandleTimeout, FailOverSec, false);
	}
	
}

void AMovingOutGameMode::PauseMatchFlow()
{
	if (!HasAuthority()) return;

	if (auto* G = GetMGameState())
	{
		G->PauseMatchTimer();
		GetWorldTimerManager().PauseTimer(TimeoutHandle);
	}
}

void AMovingOutGameMode::ResumeMatchFlow()
{
	if (!HasAuthority()) return;

	if (auto* G = GetMGameState())
	{
		G->ResumeMatchTimer();
		GetWorldTimerManager().UnPauseTimer(TimeoutHandle);
	}
}

void AMovingOutGameMode::FinishMatchFlow()
{
	if (!HasAuthority()) return;

	// 타임아웃 타이머 해제
	GetWorldTimerManager().ClearTimer(TimeoutHandle);

	if (auto* G = GetMGameState())
	{
		// 타임아웃 타이머 해제
		G->StopMatchTimer();

		// 채점
		const EMedal Medal = G->EvaluateMedal(GoldSec, SilverSec, BronzeSec, FailOverSec);

		// 결과를 G에 저장
		G->bPlayStopped = true;
		G->ResultMedal = Medal;
		G->bVictory = (Medal != EMedal::Fail);
		G->bPlayStopped = true;
		
		// 맵 이동/결과 UI 출력
		
	}
	
}



AMovingOutGameState* AMovingOutGameMode::GetMGameState() const
{
	return GetGameState<AMovingOutGameState>();
}

void AMovingOutGameMode::HandleTimeout()
{
	FinishMatchFlow();
}

void AMovingOutGameMode::UpdateTimeoutTimer()
{
	if (!HasAuthority()) return;
	if (auto* G = GetMGameState())
	{
		const float elapsed = G->GetElapsedTimeSeconds();
		const float remain  = FailOverSec - elapsed;

		auto& TM = GetWorldTimerManager();
		TM.ClearTimer(TimeoutHandle);

		if (remain > 0.f)
		{
			TM.SetTimer(TimeoutHandle, this, &AMovingOutGameMode::HandleTimeout, remain, false);
		}
		else
		{
			HandleTimeout(); // 이미 초과 → 즉시 실패 처리
		}
	}
}

