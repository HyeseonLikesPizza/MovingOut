
#include "UI/WidgetController/ResultWidgetController.h"
#include "UI/Subsystem/UIManagerSubsystem.h"
#include "Type/MedalTypes.h"

void UResultWidgetController::PushDataToWidget()
{
	if (!PC.Get()) return;
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (auto* UI = LP->GetSubsystem<UUIManagerSubsystem>())
		{
			// Result 불러오기
			Cached = UI->GetLastResult();
			const bool Victory = Cached.bVictory;

			// Medal Thresholds 가공
			TArray<FText> MedalThresholds = FormatMedalThresholdElapsed(Cached.Thresholds);

			if (Victory)
			{
				OnResultWin.Broadcast(
					FormatElapsed(Cached.ClearTimeSeconds),
					MedalThresholds,
					Cached.AdditionalGoal);
			}
			else
			{
				// Fail 위젯으로 브로드캐스트
				OnResultFail.Broadcast(
					FormatElapsed(Cached.ClearTimeSeconds),
					Cached.ItemDelivered,
					Cached.ItemTotal,
					MedalThresholds,
					Cached.AdditionalGoal);
			}

			
		}
	}
}

TArray<FText> UResultWidgetController::FormatMedalThresholdElapsed(FMedalThresholds thresholds)
{
	return {
		FormatSeconds(thresholds.GoldWithinSeconds),
		FormatSeconds(thresholds.SilverWithinSeconds),
		FormatSeconds(thresholds.BronzeWithinSeconds)
	};
}

FText UResultWidgetController::FormatSeconds(float InSeconds)
{
	int32 TotalCs = FMath::Max(0, FMath::RoundToInt(InSeconds * 100.f)); // centisecond 단위

	int32 MM =  TotalCs / 6000;        // 1분 = 6000 cs
	int32 SS = (TotalCs / 100) % 60;   // 0..59

	if (MM > 99)
	{
		MM = 99;
		SS = 59;
	}

	return FText::FromString(FString::Printf(TEXT("%d:%02d"), MM, SS));
}
