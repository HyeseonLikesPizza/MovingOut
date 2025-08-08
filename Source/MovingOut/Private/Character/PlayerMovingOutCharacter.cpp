
#include "Character/PlayerMovingOutCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MovingOut/DebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

APlayerMovingOutCharacter::APlayerMovingOutCharacter()
{
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
	GrabTraceDistance = 200.f;
	GrabDistance = 50.f;
}

void APlayerMovingOutCharacter::Tick(float DeltaSeconds)
{
	/*
	if (bIsGrabbing)
	{
		if (UPrimitiveComponent* HitComp = Hit.GetComponent())
		{
			FVector HoldLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
			FVector LeftHand = GetMesh()->GetSocketLocation(LeftHandBoneName);
			FVector RightHand = GetMesh()->GetSocketLocation(RightHandBoneName);
			FVector Middle = (LeftHand + RightHand) * 0.5f;
			FRotator MiddleRot = (RightHand - LeftHand).Rotation();
			FQuat FixedRotation = FQuat::Identity;
			//FRotator HoldRotation = GetMesh()->GetSocketRotation(GrabBoneName);
			PhysicsHandle->SetTargetLocationAndRotation(Middle + GetActorForwardVector() * GrabDistance, FixedRotation.Rotator());
		}
	}
	*/
}

void APlayerMovingOutCharacter::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ACharacter* CharacterPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	CharacterPawn->AddMovementInput(ForwardDirection, MovementVector.X);
	CharacterPawn->AddMovementInput(RightDirection, MovementVector.Y);
}

void APlayerMovingOutCharacter::TryGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("TryGrab Called"));

	//if (bIsGrabbing || PhysicsHandle->GrabbedComponent) return;

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * GrabTraceDistance;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	DrawDebugLineTrace(GetWorld(), Start, End);

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.f), Params))
	{
		if (UPrimitiveComponent* HitComp = Hit.GetComponent())
		{
			HitComp->SetSimulatePhysics(false);
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);
			HitComp->AttachToComponent(GetMesh(), Rules, RightHandBoneName);
			/*
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComp,
				RightHandBoneName,
				Hit.ImpactPoint,
				HitComp->GetComponentRotation());
			*/
			bIsGrabbing = true;
			DrawDebugHitPoint(GetWorld(), Hit);
			HitComp->SetAngularDamping(100.f);
			
		}
	}
}

void APlayerMovingOutCharacter::TryRelease()
{
	UE_LOG(LogTemp, Warning, TEXT("TryRelease Called"));
	bIsGrabbing = false;
	//Hit.GetComponent()->SetSimulatePhysics(true);
	Hit.Reset();
	//PhysicsHandle->ReleaseComponent();
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	//DetachFromActor(rules);
	//Hit.GetComponent()->DetachFromComponent(rules);
}
