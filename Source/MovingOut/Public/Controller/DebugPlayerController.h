
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DebugPlayerController.generated.h"

struct FInputActionValue;
class APlayerMovingOutCharacter;
class UInputMappingContext;
class UInputAction;

UCLASS()
class MOVINGOUT_API ADebugPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void PlayerMove(const FInputActionValue& Value);
	void Grab();
	void Release();
	void Jump();
	void StopJumping();
	void ThrowAim();
	void ThrowRelease();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputMappingContext* MappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* GrabAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	UInputAction* ThrowAction;

	

protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	APlayerMovingOutCharacter* PlayerCharacter;
};
