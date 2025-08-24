
#pragma once

#include "CoreMinimal.h"
#include "MovingOutWidget.h"
#include "PauseWidget.generated.h"


class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestRestartGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestResumeGame);


UCLASS()
class MOVINGOUT_API UPauseWidget : public UMovingOutWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRequestRestartGame OnRequestNewGame;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRequestResumeGame OnRequestResumeGame;

	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* P_Begin;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* P_End;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	

	UPROPERTY(meta=(BindWidget))
	UButton* NewGameButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* ResumeButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* EndGameButton = nullptr;

	
	
private:
	
	UFUNCTION()
	void HandleNewGameClicked();

	UFUNCTION()
	void HandleEndGameClicked();

	UFUNCTION()
	void HandleResumeClicked();
};
