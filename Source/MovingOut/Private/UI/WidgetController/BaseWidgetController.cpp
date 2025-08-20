
#include "UI/WidgetController/BaseWidgetController.h"
#include "Misc/Timespan.h"

void UBaseWidgetController::InitializeController(APlayerController* InPC, AMovingOutGameState* InGS,
	AMovingOutGameMode* InGM)
{
	PC = TWeakObjectPtr<APlayerController>(InPC);
	GS = InGS;
	GM = InGM;
}

void UBaseWidgetController::Bind()
{
}

void UBaseWidgetController::Unbind()
{
}

FText UBaseWidgetController::FormatElapsed(float Seconds) const
{
	const int32 TotalCs = FMath::Max(0, FMath::RoundToInt(Seconds * 100.f));

	int32 MM =  TotalCs / 6000;        // 1분 = 60초 = 6000 cs
	int32 SS = (TotalCs / 100) % 60;   // 0..59
	int32 CC =  TotalCs % 100;         // 0..99
	
	if (MM > 99) { MM = 99; SS = 59; CC = 99; }
	
	return FText::FromString(FString::Printf(TEXT("%d:%02d.%02d"), MM, SS, CC));
}
