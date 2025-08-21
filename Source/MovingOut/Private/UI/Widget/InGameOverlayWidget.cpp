
#include "UI/Widget/InGameOverlayWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UInGameOverlayWidget::SetWidgetController(UOverlayWidgetController* InWC)
{
	if (!InWC) return;

	if (WC)
	{
		WC->OnTimerTextChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleTimerText);
		WC->OnObjectiveChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleObjective);
		WC->OnMedalChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleMedal);
		WC->OnTimeProgressChanged.RemoveDynamic(this, &UInGameOverlayWidget::UpdateTimeProgressBar);
		WC->OnMedalChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleMedal);
	}

	WC = InWC;

	if (WC || !WC->OnTimerTextChanged.IsBound()) WC->OnTimerTextChanged.AddDynamic(this, &UInGameOverlayWidget::HandleTimerText);
	if (WC || !WC->OnObjectiveChanged.IsBound()) WC->OnObjectiveChanged.AddDynamic(this, &UInGameOverlayWidget::HandleObjective);
	if (WC || !WC->OnMedalChanged.IsBound()) WC->OnMedalChanged.AddDynamic(this, &UInGameOverlayWidget::HandleMedal);
	if (WC || !WC->OnTimeProgressChanged.IsBound()) WC->OnTimeProgressChanged.AddDynamic(this, &UInGameOverlayWidget::UpdateTimeProgressBar);
	if (WC || !WC->OnTimeProgressChanged.IsBound()) WC->OnMedalChanged.AddDynamic(this, &UInGameOverlayWidget::HandleMedal);
}

void UInGameOverlayWidget::NativeDestruct()
{
	if (WC)
	{
		WC->OnTimerTextChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleTimerText);
		WC->OnObjectiveChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleObjective);
		WC->OnMedalChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleMedal);
		WC->OnTimeProgressChanged.RemoveDynamic(this, &UInGameOverlayWidget::UpdateTimeProgressBar);
		WC->OnMedalChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleMedal);
	}
	Super::NativeDestruct();
}

void UInGameOverlayWidget::HandleTimerText(const FText& NewText)
{
	if (TimerText)
		TimerText->SetText(NewText);
}

void UInGameOverlayWidget::HandleObjective(int32 InDelivered, int32 Total)
{
	if (ObjectiveText)
	{
		const int32 Delivered = FMath::Max(0, InDelivered);
		ObjectiveText->SetText(FText::FromString(
			FString::Printf(TEXT("%d / %d"), Delivered, Total)));
	}
}

void UInGameOverlayWidget::HandleMedal(EMedal Medal)
{
	CurrentState = Medal;
}

void UInGameOverlayWidget::UpdateTimeProgressBar(float ElapsedTime, float Total)
{
	if (!TimeProgressBar || Total <= 0) return;
	const float Ratio = FMath::Clamp((float)ElapsedTime / (float)Total, 0.f, 1.f);
	TimeProgressBar->SetPercent(Ratio);
}
