
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class APlayerMovingOutCharacter;
class UCharacterMovementComponent;
class UInteractiveComponent;


UCLASS()
class MOVINGOUT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "IK", meta=(AllowPrivateAccess="true"))
	FTransform LeftHandTarget_CS;

	UPROPERTY(BlueprintReadOnly, Category = "IK", meta=(AllowPrivateAccess="true"))
	float LeftHandIKAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IK")
	float IKBlendInSpeed  = 12.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IK")
	float IKBlendOutSpeed = 12.f;
	
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APlayerMovingOutCharacter* OwningCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsGrabbing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Velocity;

private:
	
	UPROPERTY()
	UInteractiveComponent* InteractiveComponent;

	UPROPERTY()
	UMeshComponent* Mesh;

};
