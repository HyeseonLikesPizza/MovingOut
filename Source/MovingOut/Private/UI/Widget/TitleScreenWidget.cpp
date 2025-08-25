
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


