
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
	const FTimespan TS = FTimespan::FromSeconds(FMath::Max(0.f, Seconds));
	return FText::FromString(TS.ToString(TEXT("%M:%S")));
}
