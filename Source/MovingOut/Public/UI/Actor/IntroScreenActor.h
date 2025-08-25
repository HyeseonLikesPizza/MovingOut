
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntroScreenActor.generated.h"

class UAudioComponent;
class UMediaSoundComponent;

UCLASS()
class MOVINGOUT_API AIntroScreenActor : public AActor
{
	GENERATED_BODY()
/*	
public:	
	AIntroScreenActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleIntroEnded();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intro")
	UMediaSoundComponent* IntroMediaPlayer;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	UAudioComponent* AudioComponent;
	*/
};
