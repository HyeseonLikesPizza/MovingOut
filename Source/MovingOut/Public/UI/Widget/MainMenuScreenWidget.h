
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuScreenWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestNewGame);

UCLASS()
class MOVINGOUT_API UMainMenuScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRequestNewGame OnRequestNewGame;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	

	UPROPERTY(meta=(BindWidget))
	UButton* NewGameButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* EndGameButton = nullptr;
	
private:
	
	UFUNCTION()
	void HandleNewGameClicked();

	UFUNCTION()
	void HandleEndGameClicked();
	
};
