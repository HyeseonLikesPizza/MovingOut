
#include "UI/Widget/InGameOverlayWidget.h"

#include "Components/Image.h"
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
	}

	WC = InWC;

	if (WC || !WC->OnTimerTextChanged.IsBound()) WC->OnTimerTextChanged.AddDynamic(this, &UInGameOverlayWidget::HandleTimerText);
	if (WC || !WC->OnObjectiveChanged.IsBound()) WC->OnObjectiveChanged.AddDynamic(this, &UInGameOverlayWidget::HandleObjective);
	if (WC || !WC->OnMedalChanged.IsBound()) WC->OnMedalChanged.AddDynamic(this, &UInGameOverlayWidget::HandleMedal);
	if (WC || !WC->OnTimeProgressChanged.IsBound()) WC->OnTimeProgressChanged.AddDynamic(this, &UInGameOverlayWidget::UpdateTimeProgressBar);
}

void UInGameOverlayWidget::NativeDestruct()
{
	if (WC)
	{
		WC->OnTimerTextChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleTimerText);
		WC->OnObjectiveChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleObjective);
		WC->OnMedalChanged.RemoveDynamic(this, &UInGameOverlayWidget::HandleMedal);
		WC->OnTimeProgressChanged.RemoveDynamic(this, &UInGameOverlayWidget::UpdateTimeProgressBar);
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
	if (!TimerImage) return;
	
	switch (Medal)
	{
	case EMedal::Gold:
		TimerImage->SetBrushFromTexture(GoldImg);
		TimeProgressBarGold->SetFillColorAndOpacity(Color);
		break;
	case EMedal::Silver:
		TimerImage->SetBrushFromTexture(SilverImg);
		TimeProgressBarGold->SetFillColorAndOpacity(FColor::Silver);
		TimeProgressBarSilver->SetFillColorAndOpacity(FColor::Silver);
		break;
	case EMedal::Bronze:
		TimerImage->SetBrushFromTexture(BronzeImg);
		FLinearColor BronzeColor(0.67f, 0.47f, 0.3f);
		TimeProgressBarGold->SetFillColorAndOpacity(BronzeColor);
		TimeProgressBarSilver->SetFillColorAndOpacity(BronzeColor);
		TimeProgressBarBronze->SetFillColorAndOpacity(BronzeColor);
		break;
	default:
		TimerImage->SetBrushFromTexture(GoldImg);
		break;
	}
}

void UInGameOverlayWidget::UpdateTimeProgressBar(float ratio, EMedal Medal)
{
	//if (!TimeProgressBar || Total <= 0) return;
	//const float Ratio = FMath::Clamp((float)ElapsedTime / (float)Total, 0.f, 1.f);
	//TimeProgressBar->SetPercent(Ratio);

	switch (Medal)
	{
		case EMedal::Gold:
			TimeProgressBarGold->SetPercent(ratio);
			break;
		case EMedal::Silver:
			TimeProgressBarSilver->SetPercent(ratio);
			break;
		case EMedal::Bronze:
			TimeProgressBarBronze->SetPercent(ratio);
			break;
	}
}
