
#include "UI/Widget/EndGameWidget.h"

#include "Components/Image.h"
#include "UI/WidgetController/ResultWidgetController.h"
#include "Components/TextBlock.h"

void UEndGameWidget::SetWidgetController(UResultWidgetController* InWC)
{
	if (!InWC) return;

	WC = InWC;
	
	if (!WC->OnResultFail.IsBound()) WC->OnResultFail.AddDynamic(this, &UEndGameWidget::HandleResultData);
	
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
		WC->OnResultFail.RemoveDynamic(this, &UEndGameWidget::HandleResultData);
	}
	
	Super::NativeDestruct();
}

void UEndGameWidget::HandleResultData(FText ClearTimeText, int32 ItemsDelivered, int32 ItemsTotal,
	const TArray<FText>& MedalThresholds, const TArray<FAdditionalGoalData>& AdditionalGoal)
{
	SetItemsDeliveredTotal(ItemsDelivered, ItemsTotal);
	SetMedalThresholds(MedalThresholds);
	SetAdditionalGoals(AdditionalGoal);
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
		Text_ItemsTotal->SetText(Total);
	}
}

void UEndGameWidget::SetMedalThresholds(const TArray<FText>& MedalThresholds)
{
	if (Text_GoldTimeThreshold)
	{
		Text_GoldTimeThreshold->SetText(MedalThresholds[0]);
	}

	if (Text_SilverTimeThreshold)
	{
		Text_SilverTimeThreshold->SetText(MedalThresholds[1]);
	}

	if (Text_BronzeTimeThreshold)
	{
		Text_BronzeTimeThreshold->SetText(MedalThresholds[2]);
	}
}

void UEndGameWidget::SetAdditionalGoals(const TArray<FAdditionalGoalData>& InGoals)
{
	if (Text_Goal1)
	{
		FText Goal = FText::FromName(InGoals[0].GoalName);
		Text_Goal1->SetText(Goal);
	}
	if (Img_Goal1)
	{
		Img_Goal1->SetBrushFromTexture(FailIcon);
		if (InGoals[0].bCheck)
		{
			Img_Goal1->SetBrushFromTexture(SuccessIcon);	
		}
	}

	if (Text_Goal2)
	{
		FText Goal = FText::FromName(InGoals[1].GoalName);
		Text_Goal2->SetText(Goal);
	}
	if (Img_Goal2)
	{
		Img_Goal2->SetBrushFromTexture(FailIcon);
		if (InGoals[1].bCheck)
		{
			Img_Goal2->SetBrushFromTexture(SuccessIcon);	
		}
	}

	if (Text_Goal3)
	{
		FText Goal = FText::FromName(InGoals[2].GoalName);
		Text_Goal3->SetText(Goal);
	}
	if (Img_Goal3)
	{
		Img_Goal3->SetBrushFromTexture(FailIcon);
		if (InGoals[2].bCheck)
		{
			Img_Goal1->SetBrushFromTexture(SuccessIcon);	
		}
	}
}
