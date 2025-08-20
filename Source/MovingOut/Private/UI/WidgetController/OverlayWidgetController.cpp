
#include "UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidgetController::Bind()
{
	if (!GS) return;

	// 1) 목표 진행도 이벤트 바인딩
	GS->OnItemsProgress.AddDynamic(this, &UOverlayWidgetController::HandleItemsProgress);
	// 초기 한번 푸시
	HandleItemsProgress(GS->GetItemsRemaining(), GS->ItemsTotal);

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
