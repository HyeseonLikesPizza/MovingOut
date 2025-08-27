
#include "UI/Widget/ReadyWidget.h"

void UReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(R_Begin);

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]()
	{
		OnGameStart.Execute();
	}, 3.5f, false);
}
