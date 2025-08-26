
#include "UI/Widget/EndGameWinWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Type/MedalTypes.h"
#include "UI/WidgetController/ResultWidgetController.h"

void UEndGameWinWidget::SetWidgetController(UResultWidgetController* InWC)
{
	if (!InWC) return;

	WC = InWC;
	
	if (!WC->OnResultFail.IsBound()) WC->OnResultWin.AddDynamic(this, &UEndGameWinWidget::HandleResultData);
}

void UEndGameWinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	PlayAnimation(G_Begin);
}

void UEndGameWinWidget::NativeDestruct()
{
	if (WC)
    {
    	WC->OnResultWin.RemoveDynamic(this, &UEndGameWinWidget::HandleResultData);
    }
    
	Super::NativeDestruct();
}

void UEndGameWinWidget::HandleResultData(FText ClearTimeText,  const TArray<FText>& MedalThresholds, const TArray<FAdditionalGoalData>& AdditionalGoal)
{
	SetClearTimeText(ClearTimeText);
	SetMedalThresholds(MedalThresholds);
	SetAdditionalGoals(AdditionalGoal);
}

void UEndGameWinWidget::SetClearTimeText(FText ClearTimeText)
{
	if (Text_ClearTime)
	{
		Text_ClearTime->SetText(ClearTimeText);
	}
	if (Text_BestClearTime)
	{
		Text_BestClearTime->SetText(ClearTimeText);
	}
}

void UEndGameWinWidget::SetMedalThresholds(const TArray<FText>& MedalThresholds)
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

void UEndGameWinWidget::SetAdditionalGoals(const TArray<FAdditionalGoalData>& InGoals)
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
