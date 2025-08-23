
#include "UI/Widget/TitleScreenWidget.h"

UTitleScreenWidget::UTitleScreenWidget(const FObjectInitializer& OI) : Super(OI)
{
	SetIsFocusable(true);
}

void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	PlayAnimation(T_Begin);

	FTimerHandle Th;
	GetWorld()->GetTimerManager().SetTimer(
		Th,
		[this]()
		{
			PlayAnimation(T_End);
		},
		2.1f,   // 대기 초
		false   // 반복 여부
	);
}

FReply UTitleScreenWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	UE_LOG(LogTemp,Warning,TEXT("Native On Key Down"));

	if (Key == EKeys::SpaceBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Space Bar Pressed"));
		OnStartRequested.Broadcast();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}


