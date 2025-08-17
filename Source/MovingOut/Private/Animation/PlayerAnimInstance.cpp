
#include "Animation/PlayerAnimInstance.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/InteractiveComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<APlayerMovingOutCharacter>(TryGetPawnOwner());
	
	if (OwningCharacter)
	{
		MovementComponent = OwningCharacter->GetCharacterMovement();
		InteractiveComponent = OwningCharacter->InteractiveComponent;
	}



}

void UPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !MovementComponent) return;

	Velocity = OwningCharacter->GetVelocity();
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	IsGrabbing = OwningCharacter->GetIsGrabbing();
	IsFalling = MovementComponent->IsFalling();

	if (GroundSpeed == 0 && !IsGrabbing)
	{
		CarryState = EIKProfile::None;
	}
	else if (GroundSpeed == 0 && IsGrabbing && !InteractiveComponent->IsGrabbingSomething())
	{
		CarryState = EIKProfile::None;
	}
	else if (GroundSpeed == 0 && IsGrabbing && InteractiveComponent->IsGrabbingSomething())
	{
		CarryState = EIKProfile::Light;
	}
	else if (GroundSpeed > 0 && !IsGrabbing)
	{
		CarryState = EIKProfile::None;
	}
	else if (GroundSpeed > 0 && IsGrabbing && InteractiveComponent->IsGrabbingSomething())
	{
		CarryState = EIKProfile::ArmsOnly;
	}
	else if (GroundSpeed > 0 && IsGrabbing && !InteractiveComponent->IsGrabbingSomething())
	{
		CarryState = EIKProfile::None;
	}

}

void UPlayerAnimInstance::SetHipsGoal(FTransform InTransform, float PosAlpha, float RotAlpha)
{
	HipsGoal_CS = InTransform;
	HipsGoalPosAlpha = PosAlpha;
	HipsGoalRotAlpha = RotAlpha;
}
