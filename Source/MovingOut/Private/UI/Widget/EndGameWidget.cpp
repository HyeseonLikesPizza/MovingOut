
#include "UI/Widget/EndGameWidget.h"

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	PlayAnimation(G_Begin);
}
