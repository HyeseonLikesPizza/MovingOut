
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
		Mesh = OwningCharacter->GetMesh();
	}
	
}

void UPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !MovementComponent) return;

	Velocity = OwningCharacter->GetVelocity();
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	IsGrabbing = OwningCharacter->GetIsGrabbing();
	IsFalling = MovementComponent->IsFalling();

	if (OwningCharacter->bIKActive)
	{
		bIKActive = true;
		FVector Loc = OwningCharacter->LH_GoalPos_WS;
		FRotator Rot = OwningCharacter->LH_GoalRot_WS;
		LH_GoalTransform_WS.SetLocation(Loc);
		LH_GoalTransform_WS.SetRotation(Rot.Quaternion());
		LH_GoalTransform_WS.SetScale3D(FVector(1.f));
		RH_GoalRot_WS = OwningCharacter->RH_GoalRot_WS;
		RH_GoalPos_WS = OwningCharacter->RH_GoalPos_WS;
	}
	else bIKActive = false;

	

}

