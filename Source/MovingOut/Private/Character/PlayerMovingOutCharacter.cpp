
#include "Character/PlayerMovingOutCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MovingOut/DebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "MovingOut/MovingOut.h"
#include "Component/InteractiveComponent.h"

APlayerMovingOutCharacter::APlayerMovingOutCharacter()
{
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
	GrabTraceDistance = 200.f;
	GrabDistance = 50.f;

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);

	InteractiveComponent = CreateDefaultSubobject<UInteractiveComponent>(TEXT("InteractiveComponent"));
	LightCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCone"));
	LightCone->SetupAttachment(RootComponent);
	LightCone->SetVisibility(false);

}

void APlayerMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * GrabTraceDistance;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	DrawDebugLineTrace(GetWorld(), Start, End);
	bIsGrabbing = true;

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.f), Params))
	{
		
		if (UPrimitiveComponent* HitComp = Hit.GetComponent())
		{
			HitComp->SetSimulatePhysics(false);
			HitComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
			HitComp->AttachToComponent(GetMesh(), Rules, RightHandBoneName);

			/*
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComp,
				RightHandBoneName,
				Hit.ImpactPoint,
				HitComp->GetComponentRotation());
			*/
			

			DrawDebugHitPoint(GetWorld(), Hit);
			//HitComp->SetAngularDamping(100.f);
			FString Msg = FString::Printf(TEXT("Component Name : %s"), *Hit.GetComponent()->GetName());
			GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Magenta, Msg);
		}
	}
}

void APlayerMovingOutCharacter::TryRelease()
{
	bIsGrabbing = false;
	if (Hit.GetActor())
	{
		Hit.GetComponent()->SetSimulatePhysics(true);
		Hit.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		Hit.GetComponent()->DetachFromComponent(rules);
		Hit.Reset();
	}
	
	//PhysicsHandle->ReleaseComponent();
	//DetachFromActor(rules);
	
}

void APlayerMovingOutCharacter::ThrowAim()
{
	if (bIsGrabbing)
	{
		FVector Start = GetMesh()->GetSocketLocation(RightHandBoneName);
		FVector AimDir = GetActorForwardVector() * 100.f;
		float ThrowSpeed = 10.f;
		
		DrawDebugLineTrace(GetWorld(), Start, GetActorLocation()+AimDir);
		
		FPredictProjectilePathParams P;
		P.StartLocation = Start;
		P.LaunchVelocity = AimDir * ThrowSpeed;
		P.ProjectileRadius = 8.f;
		P.bTraceWithCollision = true;
		P.SimFrequency = 15.f;
		P.MaxSimTime = 2.f;
		P.TraceChannel = ECC_Visibility;

		FPredictProjectilePathResult R;
		UGameplayStatics::PredictProjectilePath(this, P, R);

		FVector AimPoint = R.HitResult.bBlockingHit ? R.HitResult.ImpactPoint : R.LastTraceDestination.Location;
		
	}
}

void APlayerMovingOutCharacter::ThrowRelease()
{
	if (!Hit.GetComponent()) return;

	Hit.GetComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Hit.GetComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Hit.GetComponent()->SetEnableGravity(true);
	Hit.GetComponent()->SetSimulatePhysics(true);

	FVector AimDir = GetActorForwardVector() * 100.f;
	float ThrowSpeed = 10.f;
	
	FVector LaunchVel = AimDir * ThrowSpeed;

	Hit.GetComponent()->SetPhysicsLinearVelocity(LaunchVel, true);

	bIsGrabbing = false;
	
}
