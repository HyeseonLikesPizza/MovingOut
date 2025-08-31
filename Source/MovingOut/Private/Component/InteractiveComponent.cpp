
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
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Props/PropsBase.h"

UInteractiveComponent::UInteractiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UInteractiveComponent::BeginPlay()
{
	Super::BeginPlay();
	Character = Cast<APlayerMovingOutCharacter>(GetOwner());
	if (Character) AddTickPrerequisiteActor(Character);
	Character->PhysicsHandle->bInterpolateTarget = false;
}

bool UInteractiveComponent::SphereTrace(const FVector& Start, const FVector& End, FHitResult& OutHit) 
{
	if (!Character) return false;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	
	return GetWorld()->SweepSingleByObjectType(OutHit, Start, End, FQuat::Identity, Props, FCollisionShape::MakeSphere(50.f), Params);
}

void UInteractiveComponent::TryGrab()
{
	if (!Character) return;
	
	Character->SetIsGrabbing(true);

	FHitResult HitResultFront;
	FHitResult HitResultLeft;

	// 1) Front 부착 지점 찾기
	FVector Start = Character->GetMesh()->GetSocketLocation(Character->GetFrontBoneName());
	FVector End = Start + Character->GetActorForwardVector() * Character->GetGrabTraceDistance();

	if (!SphereTrace(Start,End,HitResultFront)) return;
	
	if (!HitResultFront.GetComponent() || HitResultFront.GetComponent()->GetCollisionObjectType() != Props) return;

	HoldingObjData.Clear();
	HoldingObjData.Component = HitResultFront.GetComponent();
	HoldingObjData.bIsHeavy = Cast<APropsBase>(HitResultFront.GetActor())->bIsHeavy;
	Character->bIKActive = true;

	if (HoldingObjData.bIsHeavy)
	{
		Character->ProfileType = EIKProfileType::Heavy;
		// 오른손 회전값 구하기
		
		const FVector  Nr  = (-HitResultFront.ImpactNormal).GetSafeNormal();
		FVector Up = FVector::CrossProduct(Nr, FVector::RightVector).GetSafeNormal();
		const FRotator R_GripRotWS = UKismetMathLibrary::MakeRotFromXZ(Nr, Up);

		const FTransform CT = HoldingObjData.Component->GetComponentTransform();
		RH_LocalRot = (CT.InverseTransformRotation(R_GripRotWS.Quaternion())).Rotator();
		RH_LocalPos = CT.InverseTransformPosition(HitResultFront.ImpactPoint);


		// 2) impact point로 부터 HandOffset 만큼 왼쪽으로 떨어진 곳에서 왼손 부착 지점을 찾기

		FVector StartLeft = HitResultFront.ImpactPoint + Character->GetActorRightVector() * -1 * Character->HandOffset;
		FVector EndLeft = StartLeft + Character->GetActorRightVector() * -1 * Character->HandOffset;

		if (!SphereTrace(StartLeft, EndLeft, HitResultLeft)) return;
		if (HitResultLeft.GetComponent() != HoldingObjData.Component.Get()) return;

		
	
	
		const FVector P = HitResultLeft.ImpactPoint;
		const FVector  N  = HitResultLeft.ImpactNormal.GetSafeNormal();
		const FVector Forward = (-HitResultLeft.ImpactNormal).GetSafeNormal();
		const FRotator GoalRot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);

		Character->LH_GoalPos_WS = P;
		Character->LH_GoalRot_WS = GoalRot;

		const FRotator L_GripRotWS = UKismetMathLibrary::MakeRotFromXZ(-N, FVector::UpVector);
		const FTransform CompWS = HoldingObjData.Component->GetComponentTransform();
		LH_LocalPos = CompWS.InverseTransformPosition(P);
		LH_LocalRot = CompWS.InverseTransformRotation(L_GripRotWS.Quaternion()).Rotator();

		Character->PhysicsHandle->GrabComponentAtLocationWithRotation(HoldingObjData.Component, NAME_None, HitResultFront.ImpactPoint, R_GripRotWS);
	}
	else
	{
		Character->ProfileType = EIKProfileType::Light;
		Start = HitResultFront.GetComponent()->Bounds.Origin + Character->GetActorRightVector() * HitResultFront.GetComponent()->Bounds.BoxExtent;
		End = HitResultFront.GetComponent()->Bounds.Origin;

		// 오른손 값 구하기
		
		FHitResult HitRight;
		if (!SphereTrace(Start,End,HitRight) || HitResultFront.GetActor() != HitRight.GetActor()) return;
		DrawDebugSphere(GetWorld(), HitRight.ImpactPoint, 10.f, 12, FColor::Red, false, 3.f);
		
		const FRotator R_GripRotWS = Character->GetMesh()->GetSocketRotation(Character->GetRightHandBoneName());

		const FTransform CT = HoldingObjData.Component->GetComponentTransform();
		RH_LocalRot = (CT.InverseTransformRotation(R_GripRotWS.Quaternion())).Rotator();
		RH_LocalPos = CT.InverseTransformPosition(HitRight.ImpactPoint + Character->GetActorRightVector() * 10.f);

		// 왼손 값 구하기
		
		Start = HitResultFront.GetComponent()->Bounds.Origin + Character->GetActorRightVector() * -1 * HitResultFront.GetComponent()->Bounds.BoxExtent;
		End = HitResultFront.GetComponent()->Bounds.Origin;
		
		FHitResult HitLeft;
		if (!SphereTrace(Start,End,HitLeft) || HitResultFront.GetActor() != HitLeft.GetActor()) return;
		DrawDebugSphere(GetWorld(), HitLeft.ImpactPoint, 10.f, 12, FColor::Magenta, false, 2.f);

		const FVector P = HitLeft.ImpactPoint;
		const FRotator GoalRot = Character->GetMesh()->GetSocketRotation(Character->GetLeftHandBoneName());
		
		Character->LH_GoalPos_WS = P + Character->GetActorRightVector() * -1 * 10.f;
		Character->LH_GoalRot_WS = GoalRot;
		
		const FTransform CompWS = HoldingObjData.Component->GetComponentTransform();
		LH_LocalPos = CompWS.InverseTransformPosition(P);
		LH_LocalRot = CompWS.InverseTransformRotation(GoalRot.Quaternion()).Rotator();

		
		// 물체의 피직스 끄고 소켓에 붙이기
		

		HoldingObjData.Component->SetSimulatePhysics(false);
		HoldingObjData.Component->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		Character->GetCapsuleComponent()->IgnoreActorWhenMoving(HitRight.GetActor(), true);
	
		HoldingObjData.Component->IgnoreActorWhenMoving(Character, true);
		HoldingObjData.Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		HoldingObjData.Component->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, Character->GetFrontBoneName());
	}
	
}

void UInteractiveComponent::GrabRelease()
{
	if (!Character) return;
	Character->SetIsGrabbing(false);
	if (!HoldingObjData.IsEmpty())
	{
		Character->ProfileType = EIKProfileType::None;
		Character->bIKActive = false;
		Character->PhysicsHandle->ReleaseComponent();
		Character->LeftHandIKTarget->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		

		HoldingObjData.Component->IgnoreActorWhenMoving(Character, false);
		HoldingObjData.Component->SetSimulatePhysics(true);
		HoldingObjData.Component->SetEnableGravity(true);
		HoldingObjData.Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HoldingObjData.Component->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		HoldingObjData.Component->DetachFromComponent(rules);

		HoldingObjData.Clear();
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
	
	if (bIsGrabbing && !HoldingObjData.bIsHeavy)
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
	
	
	if (HoldingObjData.Component)
	{
		Character->bIKActive = false;
		Character->ProfileType = EIKProfileType::None;
		Character->bIKActive = false;
		if (Character->PhysicsHandle->GetGrabbedComponent())
			Character->PhysicsHandle->ReleaseComponent();
		HoldingObjData.Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HoldingObjData.Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HoldingObjData.Component->SetEnableGravity(true);
		HoldingObjData.Component->SetSimulatePhysics(true);

		// 시작점과 목표점
		const FVector Start = HoldingObjData.Component->GetComponentLocation();
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
			HoldingObjData.Component->SetPhysicsLinearVelocity(V0, true);
			return;
		}

		const double v2 = (g * R * R) / denom;
		const double v  = FMath::Sqrt(FMath::Max(v2, 0.0));
		
		const double YawDeg = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
		const FRotator LaunchRot(PitchDeg, YawDeg, 0.0);

		const FVector LaunchVel = LaunchRot.Vector() * v;   // cm/s
		HoldingObjData.Component->SetPhysicsLinearVelocity(LaunchVel, true);
		
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
	if (HoldingObjData.IsEmpty()) return;
	
	if (Character->bIKActive)
	{
		// 오른손
		const FTransform CT = HoldingObjData.Component->GetComponentTransform();

		// 1) 회전 앵커(로컬→월드)
		const FRotator AnchorRotWS = (CT.TransformRotation(RH_LocalRot.Quaternion())).Rotator();

		// 2) 위치 타깃: 손 소켓(손이 물체를 끌고 다님)
		const FVector HandPosWS = CT.TransformPosition(RH_LocalPos);

		// 3) 물리 핸들 타깃(보간)
		const float Follow = 12000.f;
		const FVector NewLoc = FMath::VInterpTo(Character->RH_GoalPos_WS, HandPosWS, DeltaTime, Follow);
		const FRotator NewRot = FMath::RInterpTo(Character->RH_GoalRot_WS, AnchorRotWS, DeltaTime, Follow);

		Character->RH_GoalPos_WS  = NewLoc;
		Character->RH_GoalRot_WS = NewRot;

		Character->PhysicsHandle->SetTargetLocationAndRotation(NewLoc, NewRot);
		
		
		// 왼손
		const FTransform CompWS = HoldingObjData.Component->GetComponentTransform();

		// 로컬 -> 월드 변환
		const FVector TargetPosWS = CompWS.TransformPosition(LH_LocalPos);
		const FRotator TargetRotWS = (CompWS.TransformRotation(LH_LocalRot.Quaternion())).Rotator();

		// 부드럽게 보간
		Character->LH_GoalPos_WS = FMath::VInterpTo(Character->LH_GoalPos_WS, TargetPosWS, DeltaTime, 30.f);
		Character->LH_GoalRot_WS = FMath::RInterpTo(Character->LH_GoalRot_WS, TargetRotWS, DeltaTime, 30.f);
	}
	else
	{
		
	}

	
		
	
		
}
