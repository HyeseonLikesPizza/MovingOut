
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class APlayerMovingOutCharacter;
class UCharacterMovementComponent;
class UInteractiveComponent;

UENUM(BlueprintType)
enum class EIKProfile : uint8 {
	None,
	Light,
	Heavy,
	ArmsOnly
};

UCLASS()
class MOVINGOUT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FTransform RHandTarget_CS = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FTransform LHandTarget_CS = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float HandIKAlpha = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FTransform HipsGoal_CS = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float HipsGoalPosAlpha = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float HipsGoalRotAlpha = 1.f;


	UFUNCTION(BlueprintCallable, Category = "IK")
	void SetRightHandTarget(const FTransform& InComponentSpace) { RHandTarget_CS = InComponentSpace; }
	
	UFUNCTION(BlueprintCallable, Category = "IK")
	void SetLeftHandTarget(const FTransform& InComponentSpace) { LHandTarget_CS = InComponentSpace; }
	
	UFUNCTION(BlueprintCallable, Category = "IK")
	void SetHandIKTargetsCS(const FTransform& RightCS, const FTransform& LeftCS, float Alpha = 1.f)
	{
		RHandTarget_CS = RightCS;
		LHandTarget_CS = LeftCS;
		HandIKAlpha = Alpha;
	}

	void SetHipsGoal(FTransform InTransform, float PosAlpha = 1.f, float RotAlpha = 1.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EIKProfile CarryState = EIKProfile::None;
	
	
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

	UInteractiveComponent* InteractiveComponent;

};
