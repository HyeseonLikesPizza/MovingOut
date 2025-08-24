
#include "Game/MovingOutGameMode.h"
#include "Controller/MovingOutPlayerController.h"
#include "Data/URankTimeConfig.h"
#include "Game/MovingOutGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "MovingOut/Public/Type/MedalTypes.h"
#include "Props/CountProps.h"

AMovingOutGameMode::AMovingOutGameMode()
{
	PlayerControllerClass = AMovingOutPlayerController::StaticClass();
	
	static ConstructorHelpers::FObjectFinder<UURankTimeConfig> MedalSettingsBP(TEXT("/Game/Blueprints/Data/DA_RankTimeConfig.DA_RankTimeConfig"));
	if (ensureMsgf(MedalSettingsBP.Succeeded(), TEXT("DA_Medal not found. Check path.")))
	{
		MedalThresholdDA = MedalSettingsBP.Object;
	}
	
}

void AMovingOutGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AMovingOutGameState* GS = GetGameState<AMovingOutGameState>())
	{
		if (MedalThresholdDA)
		{
			GS->MedalThresholds = MedalThresholdDA.Get()->Thresholds; // 값 복사
			//GS->ForceNetUpdate(); // 필요 시
		}
	}
}

void AMovingOutGameMode::StartPlay()
{
	Super::StartPlay();

	if (AMovingOutGameState* GS = GetGameState<AMovingOutGameState>())
	{
		GS->InitializePlacedPropsCnt();
	}
	
	StartMatchFlow();
}


void AMovingOutGameMode::StartMatchFlow()
{
	if (!HasAuthority()) return;
	auto* GS = GetGameState<AMovingOutGameState>();
	//if (!GS || GS->bTimerRunning) return;

	if (auto* G = GetMGameState())
	{
		// 시간 측정 시작
		G->StartMatchTimer();
		UE_LOG(LogTemp, Warning, TEXT("[GM] Called StartMatchTimer"));

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

