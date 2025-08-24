
#include "UI/Widget/SelectStageWidget.h"
#include "UI/Subsystem/UIManagerSubsystem.h"

void USelectStageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRequestStageInfo Broadcasted"));
		OnRequestStageInfo.Broadcast();
	}, 10.f, false);
}
