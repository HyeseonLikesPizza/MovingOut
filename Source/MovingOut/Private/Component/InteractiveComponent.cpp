
#include "Component/InteractiveComponent.h"
#include "Character/MovingOutCharacter.h"
#include "MovingOut/DebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UInteractiveComponent::UInteractiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UInteractiveComponent::TryGrab()
{
	FVector Start = Character->GetActorLocation();
	FVector End = Start + Character->GetActorForwardVector() * Character->GetGrabTraceDistance();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	DrawDebugLineTrace(GetWorld(), Start, End);
	Character->SetIsGrabbing(true);

	if (GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.f), Params))
	{

		if (UPrimitiveComponent* HitComp = HitResult.GetComponent())
		{
			HitComp->SetSimulatePhysics(false);
			HitComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
			HitComp->AttachToComponent(Character->GetMesh(), Rules, Character->GetRightHandBoneName());

			/*
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComp,
				RightHandBoneName,
				Hit.ImpactPoint,
				HitComp->GetComponentRotation());
			*/


			DrawDebugHitPoint(GetWorld(), HitResult);
			//HitComp->SetAngularDamping(100.f);
			FString Msg = FString::Printf(TEXT("Component Name : %s"), *HitResult.GetComponent()->GetName());
			GEngine->AddOnScreenDebugMessage(1, 10.f, FColor::Magenta, Msg);
		}
	}
}

void UInteractiveComponent::TryRelease()
{
	Character->SetIsGrabbing(false);
	if (HitResult.GetActor())
	{
		HitResult.GetComponent()->SetSimulatePhysics(true);
		HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		HitResult.GetComponent()->DetachFromComponent(rules);
		HitResult.Reset();
	}

	//PhysicsHandle->ReleaseComponent();
	//DetachFromActor(rules);
}

void UInteractiveComponent::ThrowAim()
{
	bool bIsGrabbing = Character->GetIsGrabbing();
	if (bIsGrabbing)
	{
		FVector Start = Character->GetMesh()->GetSocketLocation(Character->GetRightHandBoneName());
		FVector AimDir = Character->GetActorForwardVector() * 100.f;
		float ThrowSpeed = 10.f;

		DrawDebugLineTrace(GetWorld(), Start, Character->GetActorLocation() + AimDir);

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

void UInteractiveComponent::ThrowRelease()
{
	if (!HitResult.GetComponent()) return;

	HitResult.GetComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	HitResult.GetComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitResult.GetComponent()->SetEnableGravity(true);
	HitResult.GetComponent()->SetSimulatePhysics(true);

	FVector AimDir = Character->GetActorForwardVector() * 100.f;
	float ThrowSpeed = 10.f;

	FVector LaunchVel = AimDir * ThrowSpeed;

	HitResult.GetComponent()->SetPhysicsLinearVelocity(LaunchVel, true);

	Character->SetIsGrabbing(false);
}


void UInteractiveComponent::BeginPlay()
{
	Character = Cast<AMovingOutCharacter>(GetOwner());
}


void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
}

