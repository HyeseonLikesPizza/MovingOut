
#include "Component/InteractiveComponent.h"
#include "Character/MovingOutCharacter.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovingOut/DebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "MovingOut/MovingOut.h"
#include "Animation/PlayerAnimInstance.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Props/PropsBase.h"

UInteractiveComponent::UInteractiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

bool UInteractiveComponent::ProjectToObjectSurface(UPrimitiveComponent* TargetComp, const FVector& Start,
	FVector& OutPoint, FVector& OutNormal)
{
	OutPoint = Start;
	OutNormal = FVector::ForwardVector;

	if (!TargetComp) return false;

	const FVector ObjCenter = TargetComp->Bounds.Origin;

	// 작은 스피어 스윕
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Grab_Surface), false, GetOwner());
	Params.AddIgnoredActor(GetOwner());
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(10.f);

	FHitResult Hit;
	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, ObjCenter, FQuat::Identity, Props, Sphere, Params)
		&& Hit.GetComponent() == TargetComp;

	if (bHit)
	{
		OutPoint = Hit.ImpactPoint;
		OutNormal = Hit.ImpactNormal;
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.f, 10, FColor::Red, false, 3.f);
		return true;
	}
	
	return false;
}

void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<APlayerMovingOutCharacter>(GetOwner());
	AnimInstance = Cast<UPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (Character) AddTickPrerequisiteActor(Character);
}

bool UInteractiveComponent::SphereTrace(const FVector& Start, FHitResult& OutHit) 
{
	if (!Character) return false;
	
	//FVector Start = Character->GetActorLocation();
	FVector End = Start + Character->GetActorForwardVector() * Character->GetGrabTraceDistance();
	DrawDebugSphere(GetWorld(), End, 10.f, 10, FColor::Red, false, 3.f);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	
	return GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, Props, FCollisionShape::MakeSphere(100.f), Params);
}

void UInteractiveComponent::TryGrab()
{
	if (!Character) return;
	
	Character->SetIsGrabbing(true);
	FVector Start = Character->GetMesh()->GetSocketLocation(Character->GetRightHandBoneName());
	
	if (SphereTrace(Start, HitResult))
	{
		CurrentGrabbedComp = HitResult.GetComponent();
		if (CurrentGrabbedComp && CurrentGrabbedComp->GetCollisionObjectType() == Props)
		{
			// 오른손 부착
			Character->PhysicsHandle->GrabComponentAtLocation(CurrentGrabbedComp, NAME_None, HitResult.ImpactPoint);

			// 왼손 부착
			Start = Character->GetMesh()->GetSocketLocation(Character->GetLeftHandBoneName());
			FHitResult LeftHit;
			//Character->PhysicsHandle->GrabComponentAtLocation(CurrentGrabbedComp, NAME_None, LeftHit.ImpactPoint);

			if (SphereTrace(Start, LeftHit))
			{
				// 1) 손바닥이 표면을 누르는 방향(= -ImpactNormal)
				const FVector PalmForward = (-LeftHit.ImpactNormal).GetSafeNormal();

				// 2) 캐릭터 업벡터를 Up 기준으로 회전 구성
				const FVector UpRef = Character->GetActorUpVector();
				const FRotator PalmRot = FRotationMatrix::MakeFromXZ(PalmForward, UpRef).Rotator();

				// 3) IK 타깃을 임팩트 지점/회전에 놓기(살짝 띄우고 싶으면 +Normal*2.f 등)
				Character->LeftHandIKTarget->SetWorldLocationAndRotation(LeftHit.ImpactPoint, PalmRot);

				// 4) IK 켜기(애님 그래프에서 쓸 Bool/Alpha)
				//Character->bLeftHandIK = true;   // 캐릭터에 UPROPERTY(BlueprintReadOnly) bool 하나만 추가해두면 편함
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Left Hand Sphere Trace Failed"));
			}

			

			//Character->LeftHandIKTarget->SetWorldLocation(LeftHit.ImpactPoint);

			
			// 왼손 부착
			
		}
	}

	
}

void UInteractiveComponent::GrabRelease()
{
	Character->SetIsGrabbing(false);
	if (IsHoldingObject())
	{
		Character->PhysicsHandle->ReleaseComponent();
		
		//Character->GetCapsuleComponent()->IgnoreActorWhenMoving(HitResult.GetActor(), false);
		CurrentGrabbedComp->IgnoreActorWhenMoving(Character, false);
		CurrentGrabbedComp->SetSimulatePhysics(true);
		CurrentGrabbedComp->SetEnableGravity(true);
		CurrentGrabbedComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentGrabbedComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		CurrentGrabbedComp->DetachFromComponent(rules);
		CurrentGrabbedComp = nullptr;
		HitResult.Reset();
	}

	if (IsAming)
	{
		IsAming = false;
		ThrowRelease();
		CancelThrowAming();
	}
	
	
}

void UInteractiveComponent::ThrowAim()
{
	bool bIsGrabbing = Character->GetIsGrabbing();
	
	if (bIsGrabbing)
	{
		SetThrowIndicatorVisible(true);
		IsAming = true;

		
		FVector Start = Character->GetMesh()->GetSocketLocation(Character->GetRightHandBoneName());
		FVector AimDir = Character->GetActorForwardVector() * 100.f;

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

		AimPoint = R.HitResult.bBlockingHit ? R.HitResult.ImpactPoint : R.LastTraceDestination.Location;

	}
}

void UInteractiveComponent::ThrowRelease()
{
	IsAming = false;

	Character->LightCone->SetVisibility(false);
	Character->CrosshairDecal->SetVisibility(false, true);
	
	if (CurrentGrabbedComp)
	{
		CurrentGrabbedComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		CurrentGrabbedComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentGrabbedComp->SetEnableGravity(true);
		CurrentGrabbedComp->SetSimulatePhysics(true);

		// 시작점과 목표점
		const FVector Start = CurrentGrabbedComp->GetComponentLocation();
		const FVector Target = Character->CrosshairDecal->GetComponentLocation();
		const FVector ToTarget = Target - Start;

		// 수평면(XY)에서의 거리와 Yaw
		const double R   = FVector(ToTarget.X, ToTarget.Y, 0).Length();
		const double dZ  = ToTarget.Z;
		const double g   = -GetWorld()->GetGravityZ();      // 양수

		// 네가 원하는 발사 피치(수평 기준 각도, +면 위로)
		const double PitchDeg = DesiredPitchDegrees;        // 예: 25.0
		const double theta    = FMath::DegreesToRadians(PitchDeg);

		const double cosT = FMath::Cos(theta);
		const double sinT = FMath::Sin(theta);
		const double tanT = FMath::Tan(theta);

		const double denom = (R * tanT - dZ) * 2.0 * cosT * cosT;

		if (R < KINDA_SMALL_NUMBER || denom <= 0.0 || cosT == 0.0)
		{
			// 이 각도로는 해가 없음: 각도를 높이거나(더 포물선), 다른 방식으로 계산
			// 안전한 폴백: 비행시간 T 기반 방식(이전 메시지)으로 V0 계산
			const double T = FMath::Clamp(DesiredFlightTimeSeconds, 0.25, 3.0);
			const FVector V0 = (ToTarget / T) - 0.5 * FVector(0,0,-g) * T;
			CurrentGrabbedComp->SetPhysicsLinearVelocity(V0, true);
			return;
		}

		const double v2 = (g * R * R) / denom;
		const double v  = FMath::Sqrt(FMath::Max(v2, 0.0));

		// Yaw 는 목표를 바라보게, Pitch 는 네가 정한 각도를 사용
		const double YawDeg = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
		const FRotator LaunchRot(PitchDeg, YawDeg, 0.0);

		const FVector LaunchVel = LaunchRot.Vector() * v;   // cm/s
		CurrentGrabbedComp->SetPhysicsLinearVelocity(LaunchVel, true);
		
	}
	
	
	Character->SetIsGrabbing(false);

	
}

void UInteractiveComponent::SetThrowIndicatorVisible(bool bVisible)
{
	if (Character)
	{
		Character->CrosshairDecal->SetVisibility(bVisible, true);
		Character->LightCone->SetVisibility(bVisible);	
	}
}

void UInteractiveComponent::CancelThrowAming()
{
	IsAming = false;
	SetThrowIndicatorVisible(false);
}

void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	if (CurrentGrabbedComp)
	{
		// 오른손
		FTransform R_HandSocketTransform = Character->GetMesh()->GetSocketTransform(Character->GetRightHandBoneName());
		Character->PhysicsHandle->SetTargetLocationAndRotation(R_HandSocketTransform.GetLocation(), R_HandSocketTransform.GetRotation().Rotator());


		// 왼손

		const float TargetAlpha = (CurrentGrabbedComp != nullptr) ? 1.f : 0.f;
		const float Speed = (TargetAlpha > LeftHandIKAlpha) ? IKBlendInSpeed : IKBlendOutSpeed;
		LeftHandIKAlpha = FMath::FInterpTo(LeftHandIKAlpha, TargetAlpha, DeltaTime, Speed);

		if (CurrentGrabbedComp)
		{
			FTransform LeftWorld;
			
		}
	}
		
}
