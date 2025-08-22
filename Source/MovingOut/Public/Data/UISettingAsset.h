
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UISettingAsset.generated.h"

UCLASS()
class MOVINGOUT_API UUISettingAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> TitleScreenWidgetClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> MainMenuWidgetClass;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> OverlayHUDClass;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<UUserWidget> ResultWidgetClass;
};
