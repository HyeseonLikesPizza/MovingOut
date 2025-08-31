
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

	// Front 부착 지점 찾기
	FVector Start = Character->GetMesh()->GetSocketLocation(Character->GetFrontBoneName());
	FVector End = Start + Character->GetActorForwardVector() * Character->GetGrabTraceDistance();
	
	if (SphereTrace(Start, End, HitResultFront))
	{	
		if (HitResultFront.GetComponent() && HitResultFront.GetComponent()->GetCollisionObjectType() == Props)
		{
			HoldingObjData.Clear();
			HoldingObjData.Component = HitResultFront.GetComponent();
			HoldingObjData.bIsHeavy = Cast<APropsBase>(HitResultFront.GetActor())->bIsHeavy;

			// 무거운 물건일 때
			
			

			// 오른손 회전값 구하기
			const FVector  Nr  = (-HitResultFront.ImpactNormal).GetSafeNormal();
			FVector Up = FVector::CrossProduct(Nr, FVector::RightVector).GetSafeNormal();
			const FRotator R_GripRotWS = UKismetMathLibrary::MakeRotFromXZ(Nr, Up);

			const FTransform CT = HoldingObjData.Component->GetComponentTransform();
			RH_LocalRot = (CT.InverseTransformRotation(R_GripRotWS.Quaternion())).Rotator();
			RH_LocalPos = CT.InverseTransformPosition(HitResultFront.ImpactPoint);
		
		
			// impact point로 부터 HandOffset 만큼 왼쪽으로 떨어진 곳에서 왼손 부착 지점을 찾기
		
			FVector StartLeft = HitResultFront.ImpactPoint + Character->GetActorRightVector() * -1 * Character->HandOffset;
			FVector EndLeft = StartLeft + Character->GetActorRightVector() * -1 * Character->HandOffset;

			if (!HoldingObjData.bIsHeavy)
				StartLeft = HitResultFront.ImpactPoint + Character->GetActorRightVector() * -1 * 10.f;

			if (SphereTrace(StartLeft, EndLeft, HitResultLeft))
			{
				Character->bIKActive = true;
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


				// 오른손 피직스 핸들 설정
				// 무거운 물건일 때
				if (HoldingObjData.bIsHeavy)
				{
					
					Character->PhysicsHandle->GrabComponentAtLocationWithRotation(HoldingObjData.Component, NAME_None, HitResultFront.ImpactPoint, R_GripRotWS);
				}
				else
				{
					// 가벼운 물건일 때
					//Character->bIKActive = false;

					HoldingObjData.Component->SetSimulatePhysics(false);
					HoldingObjData.Component->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
					FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
					Character->GetCapsuleComponent()->IgnoreActorWhenMoving(HitResultFront.GetActor(), true);
			
					HoldingObjData.Component->IgnoreActorWhenMoving(Character, true);
					HoldingObjData.Component->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
					HoldingObjData.Component->AttachToComponent(Character->GetMesh(), Rules, Character->GetRightHandBoneName());
				}
			}
			
			
			

			
			
			
		}
	}

	
}

void UInteractiveComponent::GrabRelease()
{
	if (!Character) return;
	Character->SetIsGrabbing(false);
	if (!HoldingObjData.IsEmpty())
	{
		Character->bIKActive = false;
		Character->PhysicsHandle->ReleaseComponent();
		Character->bLeftHandIK = false;
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
	
	
	if (HoldingObjData.Component)
	{
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
	
	if (!HoldingObjData.IsEmpty())
	{
		// 오른손
		const FTransform CT = HoldingObjData.Component->GetComponentTransform();

		// 1) 회전 앵커(로컬→월드)
		const FRotator AnchorRotWS = (CT.TransformRotation(RH_LocalRot.Quaternion())).Rotator();

		// 2) 위치 타깃: 손 소켓(손이 물체를 끌고 다님)
		const FVector HandPosWS = Character->GetMesh()
			->GetSocketTransform(Character->GetFrontBoneName()).GetLocation();

		// 3) 물리 핸들 타깃(보간)
		const float Follow = 12000.f;
		const FVector NewLoc = FMath::VInterpTo(Character->RH_GoalPos_WS, HandPosWS, DeltaTime, Follow);
		const FRotator NewRot = FMath::RInterpTo(Character->RH_GoalRot_WS, AnchorRotWS, DeltaTime, Follow);

		Character->RH_GoalPos_WS  = NewLoc;
		Character->RH_GoalRot_WS = NewRot;

		Character->PhysicsHandle->SetTargetLocationAndRotation(NewLoc, NewRot);
		
		
		// 왼손
		if (Character->bIKActive)
		{
			const FTransform CompWS = HoldingObjData.Component->GetComponentTransform();

			// 로컬 -> 월드 변환
			const FVector TargetPosWS = CompWS.TransformPosition(LH_LocalPos);
			const FRotator TargetRotWS = (CompWS.TransformRotation(LH_LocalRot.Quaternion())).Rotator();

			// 부드럽게 보간
			Character->LH_GoalPos_WS = FMath::VInterpTo(Character->LH_GoalPos_WS, TargetPosWS, DeltaTime, 30.f);
			Character->LH_GoalRot_WS = FMath::RInterpTo(Character->LH_GoalRot_WS, TargetRotWS, DeltaTime, 30.f);
		}
		
		
	}
		
}
