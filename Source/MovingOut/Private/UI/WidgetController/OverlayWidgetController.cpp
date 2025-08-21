
#include "UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidgetController::Bind()
{
	if (!GS) return;

	// 1) 목표 진행도 이벤트 바인딩
	GS->OnItemsProgress.AddDynamic(this, &UOverlayWidgetController::HandleItemsProgress);

	
	StartTimeCache = GS->RunningStartTime;
	
	
	// 초기 한번 푸시
	HandleItemsProgress(GS->ItemsDelivered, GS->PlacedPropsCnt);

	// 2) 타이머 텍스트 갱신(루프 타이머) — 0.1~0.25s 추천
	if (UWorld* World = GS->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TimerHandle_UIRefresh, this, &UOverlayWidgetController::TickUI, 0.1f, true, 0.f
		);
	}

	// 3) 상태 캐시 초기화
	bLastPlayStopped = GS->bPlayStopped;
	LastMedal        = GS->ResultMedal;
	
	// 초기 UI 풀 푸시
	TickUI();
}

void UOverlayWidgetController::Unbind()
{
	if (GS)
	{
		GS->OnItemsProgress.RemoveDynamic(this, &UOverlayWidgetController::HandleItemsProgress);
		if (UWorld* World = GS->GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimerHandle_UIRefresh);
		}
	}
}

void UOverlayWidgetController::TickUI()
{
	if (!GS) return;

	// 1) 경과시간 텍스트
	const float Elapsed = GS ? GS->GetElapsedTimeSeconds() : -1.f;
	const float Now = GS ? GS->GetServerWorldTimeSeconds() : -1.f;
	UE_LOG(LogTemp, Warning, TEXT("[WC] Now=%.2f Elapsed=%.2f Run=%d Start=%.2f Acc=%.2f"),
		Now, Elapsed, GS?GS->bTimerRunning:0, GS?GS->RunningStartTime:0.f, GS?GS->AccumulatedSeconds:0.f);
	OnTimerTextChanged.Broadcast(FormatElapsed(Elapsed));


	// 프로그레스바 업데이트
	const float FailOverSec = GS->MedalThresholds.FailOverSeconds;
	OnTimeProgressChanged.Broadcast(Elapsed, FailOverSec);

	// 이미지 업데이트 (EMedal 전달)
 	EMedal Medal = ComputeStage(Elapsed);
	if (LastMedal != Medal)
	{
		OnMedalChanged.Broadcast(Medal);
		
		const UEnum* MedalEnum = StaticEnum<EMedal>();
		const FString MedalStr = MedalEnum
			? MedalEnum->GetNameStringByValue(static_cast<int64>(Medal)) // "Gold" 같은 이름만
			: TEXT("Invalid");

		GEngine->AddOnScreenDebugMessage(
			/*Key*/-1, /*Time*/3.f, FColor::Blue,
			FString::Printf(TEXT("Medal: %s"), *MedalStr)
		);
		LastMedal = Medal;
	}

	// 2) 메달/종료 상태 변화 감지해 한 번만 푸시
	if (GS->bPlayStopped != bLastPlayStopped || GS->ResultMedal != LastMedal)
	{
		bLastPlayStopped = GS->bPlayStopped;
		LastMedal        = GS->ResultMedal;
		OnMedalChanged.Broadcast(LastMedal);
	}
}

void UOverlayWidgetController::HandleItemsProgress(int32 Remaining, int32 Total)
{
	OnObjectiveChanged.Broadcast(Remaining, Total);
}

EMedal UOverlayWidgetController::ComputeStage(float ElapsedSecond) const
{
	if (!GS) return EMedal::None;

	if (ElapsedSecond <= GS->MedalThresholds.GoldWithinSeconds)
		return EMedal::Gold;
	if (ElapsedSecond <= GS->MedalThresholds.SilverWithinSeconds)
		return EMedal::Silver;
	if (ElapsedSecond <= GS->MedalThresholds.BronzeWithinSeconds)
		return EMedal::Bronze;
	if (ElapsedSecond <= GS->MedalThresholds.FailOverSeconds)
		return EMedal::Fail;

	return EMedal::None;
}

