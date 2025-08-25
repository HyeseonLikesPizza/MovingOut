
#include "UI/Widget/IntroWidget.h"

void UIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}


FReply UIntroWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(MyGeometry, InKeyEvent);

	UE_LOG(LogTemp, Warning, TEXT("Intro Native On Key Down"));

	const FKey Key = InKeyEvent.GetKey();
	
	OnRequestSelectLevel.Broadcast();
	
	return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}

