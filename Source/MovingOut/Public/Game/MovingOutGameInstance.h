
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MovingOutGameInstance.generated.h"


UCLASS()
class MOVINGOUT_API UMovingOutGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

private:

	UFUNCTION()
	void BeginLoadingScreen(const FString& MapName);

	UFUNCTION()
	void EndLoadingScreen(UWorld* LoadedWorld);

	UFUNCTION()
	void BeginLoadingScreen_WithContext(const FWorldContext& inWorldContext, const FString& MapName);

	UPROPERTY()
	TWeakObjectPtr<class UUserWidget> LoadingUMG;
};
