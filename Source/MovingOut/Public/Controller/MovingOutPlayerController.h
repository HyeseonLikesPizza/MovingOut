
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MovingOutPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UTitleScreenWidget;
class APlayerMovingOutCharacter;



UCLASS()
class MOVINGOUT_API AMovingOutPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMovingOutPlayerController();
	
	virtual void BeginPlay() override;

protected:
	virtual void SetupInputComponent() override;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputMappingContext* MappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* ReleaseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* ThrowAction;


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* PauseAction;
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* ResumeAction;
	*/
	
	void PlayerMove(const FInputActionValue& Value);
	void Grab();
	void Release();
	void Jump();
	void StopJumping();
	void ThrowAim();
	void ThrowRelease();


private:

	APlayerMovingOutCharacter* PlayerCharacter;


	void HandleESCPressed();
};
