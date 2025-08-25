
#include "UI/Widget/EndGameWidget.h"
#include "UI/WidgetController/ResultWidgetController.h"
#include "Components/TextBlock.h"

void UEndGameWidget::SetWidgetController(UResultWidgetController* InWC)
{
	if (!InWC) return;

	WC = InWC;
	
	if (!WC->Pushed.IsBound()) WC->Pushed.AddDynamic(this, &UEndGameWidget::HandleResultData);
	
}

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	PlayAnimation(G_Begin);
}

void UEndGameWidget::NativeDestruct()
{
	if (WC)
	{
		WC->Pushed.RemoveDynamic(this, &UEndGameWidget::HandleResultData);
	}
	
	Super::NativeDestruct();
}

void UEndGameWidget::HandleResultData(bool bVictory, FText ClearTimeText, int32 ItemsDelivered, int32 ItemsTotal,
	FMedalThresholds MedalThresholds)
{
	SetClearTimeText(ClearTimeText);
	SetItemsDeliveredTotal(ItemsDelivered, ItemsTotal);
}

void UEndGameWidget::SetClearTimeText(FText ClearTimeText)
{
	if (Text_ClearTime)
	{
		Text_ClearTime->SetText(ClearTimeText);
	}
}

void UEndGameWidget::SetVictoryImage(bool bVictory)
{
}

void UEndGameWidget::SetItemsDeliveredTotal(int32 ItemsDelivered, int32 ItemsTotal)
{
	if (Text_ItemsDelivered)
	{
		FText Delivered = FText::FromString(FString::Printf(TEXT("%d"), ItemsDelivered));
		Text_ItemsDelivered->SetText(Delivered);
	}

	if (Text_ItemsTotal)
	{
		FText Total = FText::FromString(FString::Printf(TEXT("%d"), ItemsTotal));
		Text_ItemsDelivered->SetText(Total);
	}
}

void UEndGameWidget::SetMedalThresholds(FMedalThresholds MedalThresholds)
{
}
