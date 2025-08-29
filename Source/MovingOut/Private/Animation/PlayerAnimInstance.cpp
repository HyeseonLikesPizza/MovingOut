
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

	
	FTransform MeshWorld = Mesh->GetComponentTransform();
	FTransform TargetWorld = OwningCharacter->LeftHandIKTarget->GetComponentTransform();
	

}

