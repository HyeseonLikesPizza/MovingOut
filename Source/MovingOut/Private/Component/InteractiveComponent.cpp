
#include "Component/InteractiveComponent.h"
#include "Character/MovingOutCharacter.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovingOut/DebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "MovingOut/MovingOut.h"
#include "Animation/PlayerAnimInstance.h"
#include "Components/CapsuleComponent.h"

UInteractiveComponent::UInteractiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UInteractiveComponent::BeginPlay()
{
	Character = Cast<AMovingOutCharacter>(GetOwner());
	AnimInstance = Cast<UPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (Character) AddTickPrerequisiteActor(Character);
}

// 캐리 프레임(월드): 골반 위치, Yaw=Actor Forward, Up=+Z
FTransform UInteractiveComponent::MakeCarryFrame()
{
	const FVector Hips = Character->GetMesh()->GetSocketLocation(TEXT("Hips"));
	const FVector fwd = Character->GetActorForwardVector();
	const FVector up = FVector::UpVector;
	const FQuat   rot = FRotationMatrix::MakeFromXZ(fwd, up).ToQuat();
	return FTransform(rot, Hips);
}

// 소켓 이름에서 라벨 뽑기
static FString LabelFrom(const FString& Name) {
	if (Name.Contains(TEXT("Forward"), ESearchCase::IgnoreCase)) return TEXT("Forward");
	if (Name.Contains(TEXT("Backward"), ESearchCase::IgnoreCase)) return TEXT("Backward");
	if (Name.Contains(TEXT("Left"), ESearchCase::IgnoreCase)) return TEXT("Left");
	if (Name.Contains(TEXT("Right"), ESearchCase::IgnoreCase)) return TEXT("Right");
	return TEXT("");
}

// 라벨 → 해당 면의 월드 법선
static FVector FaceNormalWS(const FString& Label, const FTransform& CompTM) {
	if (Label == "Forward")  return  CompTM.GetUnitAxis(EAxis::X);
	if (Label == "Backward") return -CompTM.GetUnitAxis(EAxis::X);
	if (Label == "Right")    return  CompTM.GetUnitAxis(EAxis::Y);
	if (Label == "Left")     return -CompTM.GetUnitAxis(EAxis::Y);
	return CompTM.GetUnitAxis(EAxis::X); // fallback
}

// N=면 법선, T=엣지 방향(+Z). 손 로컬 +X(손바닥 법선) = -N, 손 로컬 +Y(손가락) = T 로 맞춤.
// 손 스켈레톤에 따라 오프셋으로 미세조정 가능.
static FQuat MakePalmFacingQuat(const FVector& N_ws, const FVector& T_ws, const FRotator& HandOffset)
{
	const FVector N = N_ws.GetSafeNormal();
	FVector T = (T_ws - FVector::DotProduct(T_ws, N) * N).GetSafeNormal(); // N에 직교화

	// 로컬 +X(손바닥) = -N, 로컬 +Y(손가락) = T
	const FQuat basisQ = FRotationMatrix::MakeFromXY(-N, T).ToQuat(); // ← 여기!
	return basisQ * HandOffset.Quaternion();
}


void UInteractiveComponent::TryGrab()
{
	UE_LOG(LogTemp, Warning, TEXT("TryGrab Called"));
	if (!Character) return;
	FVector Start = Character->GetActorLocation();
	FVector End = Start + Character->GetActorForwardVector() * Character->GetGrabTraceDistance();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	DrawDebugLineTrace(GetWorld(), Start, End);
	Character->SetIsGrabbing(true);

	if (GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, Props, FCollisionShape::MakeSphere(200.f), Params))
	{

		if (UPrimitiveComponent* HitComp = HitResult.GetComponent())
		{
			if (HitComp->GetCollisionObjectType() == Props)
			{
				HitComp->SetSimulatePhysics(false);
				HitComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
				FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				Character->GetCapsuleComponent()->IgnoreActorWhenMoving(HitResult.GetActor(), true);
				
				HitComp->IgnoreActorWhenMoving(Character, true);
				HitComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

				PickFaceEdgesAndSetIK();
				
			
			}
			
		}
	}
}

void UInteractiveComponent::GrabRelease()
{
	UE_LOG(LogTemp, Warning, TEXT("TryRelease Called"));
	Character->SetIsGrabbing(false);
	if (HitResult.GetActor())
	{
		Character->GetCapsuleComponent()->IgnoreActorWhenMoving(HitResult.GetActor(), false);
		HitResult.GetComponent()->IgnoreActorWhenMoving(Character, false);
		HitResult.GetComponent()->SetSimulatePhysics(true);
		HitResult.GetComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		HitResult.GetComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		HitResult.GetComponent()->DetachFromComponent(rules);
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
	UE_LOG(LogTemp, Warning, TEXT("ThrowAim Called"));
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
	UE_LOG(LogTemp, Warning, TEXT("ThrowRelease Called"));
	IsAming = false;

	Character->LightCone->SetVisibility(false);
	Character->CrosshairDecal->SetVisibility(false, true);
	
	if (HitResult.GetComponent())
	{
		HitResult.GetComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HitResult.GetComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HitResult.GetComponent()->SetEnableGravity(true);
		HitResult.GetComponent()->SetSimulatePhysics(true);
		FVector AimDir = Character->GetActorForwardVector();
		FRotator AimRot = AimDir.Rotation();
		AimRot.Pitch += ThrowAngle;
		FVector ThrowDir = AimRot.Vector();
		FVector LaunchVel = ThrowDir * ThrowSpeed * 10.f;
		HitResult.GetComponent()->SetPhysicsLinearVelocity(LaunchVel, true);
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

bool UInteractiveComponent::PickFaceEdgesAndSetIK()
{
	UE_LOG(LogTemp, Warning, TEXT("AttachToPropCornor Called"));
	if (!HitResult.GetComponent() || !Character) return false;
    UPrimitiveComponent* Comp = HitResult.GetComponent();
    if (!Comp) return false;



	const FTransform CompTM = Comp->GetComponentTransform();

	// 1) 필요한 소켓들만 모으기
	auto FindSocket = [&](const TCHAR* Key)->TOptional<FTransform>
		{
			TArray<FName> Names; 
			Names = Comp->GetAllSocketNames();
			for (const FName& N : Names)
			{
				if (N.ToString().Contains(Key, ESearchCase::IgnoreCase))
				{
					return Comp->GetSocketTransform(N, RTS_World);
				}
			}
			return TOptional<FTransform>();
		};

	const TOptional<FTransform> SockF = FindSocket(TEXT("Forward"));
	const TOptional<FTransform> SockB = FindSocket(TEXT("Backward"));
	const TOptional<FTransform> SockL = FindSocket(TEXT("Left"));
	const TOptional<FTransform> SockR = FindSocket(TEXT("Right"));

	// 최소한 한 축 페어는 있어야 함
	const bool HasXPair = SockF.IsSet() && SockB.IsSet();
	const bool HasYPair = SockL.IsSet() && SockR.IsSet();
	if (!HasXPair && !HasYPair) return false;

	// 2) 플레이어가 어느 "면"에 더 가까운지(로컬 좌표로 판정)
	const FVector PlayerLS = CompTM.InverseTransformPosition(Character->GetActorLocation());

	// 앞/뒤(±X)와 좌/우(±Y)까지의 거리
	// 소켓들이 엣지 중간점이면 min/max 대신 단순히 X/Y의 절대값 비교로 충분
	const float dX = FMath::Abs(PlayerLS.X);
	const float dY = FMath::Abs(PlayerLS.Y);

	enum class EAxis { X, Y };
	EAxis UseAxis;
	if (!HasYPair) UseAxis = EAxis::X;
	else if (!HasXPair) UseAxis = EAxis::Y;
	else UseAxis = (dX >= dY) ? EAxis::Y : EAxis::X; // 앞/뒤에 더 가까우면 Y축 페어를, 좌/우에 더 가까우면 X축 페어를

	// 3) 교차 매핑
	FTransform L_WS, R_WS;
	if (UseAxis == EAxis::Y) // Left/Right 페어 사용 → 교차: L=Right, R=Left
	{
		if (!HasYPair) return false;
		L_WS = SockR.GetValue(); // 왼손이 Right
		R_WS = SockL.GetValue(); // 오른손이 Left
		SetGripMidPoint(FName("Edge_Left"), FName("Edge_Right"));
	}
	else // EAxis::X  → Forward/Backward 페어 사용 → 교차: L=Forward, R=Backward
	{
		if (!HasXPair) return false;
		L_WS = SockF.GetValue(); // 왼손이 Forward
		R_WS = SockB.GetValue(); // 오른손이 Backward
		SetGripMidPoint(FName("Edge_Backward"), FName("Edge_Forward"));
	}

	// 4) IK 타깃 전달 (AnimBP에서 Component Space로 변환해서 쓰기)
	AnimInstance->SetLeftHandTarget(L_WS);
	AnimInstance->SetRightHandTarget(R_WS);

	

	

    // (선택) 디버그
    //DrawDebugSphere(GetWorld(), LEdge_WS.GetLocation(), 6.f, 12, FColor::Green, false, 1.f);
    //DrawDebugSphere(GetWorld(), REdge_WS.GetLocation(), 6.f, 12, FColor::Blue,  false, 1.f);

    return true;

}


void UInteractiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	switch (AnimInstance->CarryState)
	{
	case EIKProfile::Light:
		TickCarry_Light(DeltaTime, Settings);
		break;
	case EIKProfile::ArmsOnly:
		TickCarry_MoveCoupled(DeltaTime);
		break;
	}

	
}

void UInteractiveComponent::TickCarry_Light(float DeltaTime, const FCarrySettings& S)
{
	if (!HitResult.GetComponent() || !Character->GetIsGrabbing()) return;

	UPrimitiveComponent* Comp = HitResult.GetComponent();
	const FTransform CompTM = Comp->GetComponentTransform();

	// 1) 목표 "그립 기준점" 위치(플레이어 앞)
	const FVector PelvisWS = Character->GetMesh()->GetBoneLocation(TEXT("Hips")); // 또는 캡슐/메시 위치
	const FVector Fwd = Character->GetActorForwardVector();
	const FVector Up = FVector::UpVector;

	FTransform DesiredMid_WS;
	DesiredMid_WS.SetLocation(PelvisWS + Fwd * S.CarryDist + Up * S.CarryHeight);

	// (선택) 오브젝트가 플레이어를 향하게: Yaw만 정렬
	const FRotator Face = FRotationMatrix::MakeFromXZ(-Fwd, Up).Rotator(); // 전면이 플레이어를 보게
	DesiredMid_WS.SetRotation(Face.Quaternion());

	// 2) 오브젝트 새 트랜스폼 = 목표 그립 * (로컬 그립)^-1
	const FTransform Curr = Comp->GetComponentTransform();
	const FTransform TargetWS = DesiredMid_WS * GripLocal.Inverse();

	// 3) 부드럽게 보간 + 텔레포트 설정(스윕 X)
	const FVector NewLoc = FMath::VInterpTo(Curr.GetLocation(), TargetWS.GetLocation(), DeltaTime, S.MovePosSpeed);
	const FQuat   NewRot = FMath::QInterpTo(Curr.GetRotation(), TargetWS.GetRotation(), DeltaTime, S.MoveRotSpeed);
	FTransform NewTM(NewRot, NewLoc, Curr.GetScale3D());

	Comp->SetWorldTransform(NewTM, /*bSweep=*/false, nullptr, ETeleportType::TeleportPhysics);

	// 4) 손 IK 목표 갱신 (회전 포함)
	const FTransform R_SocketWS = Comp->GetSocketTransform(RightSocketName, RTS_World);
	const FTransform L_SocketWS = Comp->GetSocketTransform(LeftSocketName, RTS_World);

	// 라벨 → 면 법선
	const FString RLabel = LabelFrom(RightSocketName.ToString());
	const FString LLabel = LabelFrom(LeftSocketName.ToString());
	const FVector N_R = FaceNormalWS(RLabel, CompTM);
	const FVector N_L = FaceNormalWS(LLabel, CompTM);

	// 엣지 방향은 세로(+Z)라 가정 (나중에 Top/Bottom 엣지를 쓰면 여기서 분기)
	const FVector EdgeDir = CompTM.GetUnitAxis(EAxis::Z);

	// 손 스켈레톤 보정(한 번만 맞추면 됨)
	//   - 만약 네 스켈레톤이 "손바닥 법선=로컬 +X, 손가락=로컬 +Y"가 아니라면
	//     아래 Offset을 바꿔서 맞춰줘.
	const FRotator HandOffset_R = FRotator(0, 90.f, 0.f);     // 필요시 미세조정
	const FRotator HandOffset_L = FRotator(0, -90.f, 90.f);     // 왼손이 뒤집혀 보이면 Yaw 180 등

	FTransform R_WS = R_SocketWS;
	FTransform L_WS = L_SocketWS;
	R_WS.SetRotation(MakePalmFacingQuat(N_R, EdgeDir, HandOffset_R));
	L_WS.SetRotation(MakePalmFacingQuat(N_L, EdgeDir, HandOffset_L));

	AnimInstance->SetRightHandTarget(R_WS);
	AnimInstance->SetLeftHandTarget(L_WS);

	// 5) 애님 프로파일: Light 사용
	//AnimInstance->bUseLightCarryProfile = true;
}

void UInteractiveComponent::TickCarry_Heavy(float DeltaTime, const FCarrySettings& S)
{

}

void UInteractiveComponent::TickCarry_MoveCoupled(float DeltaTime, float posSpeed, float rotSpeed)
{
	if (!HitResult.GetComponent()) return;
	UPrimitiveComponent* HeldComp = HitResult.GetComponent();

	// ① 목표 오브젝트 변환 = (새 캐리프레임) * (저장된 상대)
	const FTransform carryWS = MakeCarryFrame();
	const FTransform targetWS = RelObjFromCarry * carryWS; // 또는 carryWS * RelObjFromCarry (UE 버전에 맞게 확인)
	const FTransform curr = HeldComp->GetComponentTransform();

	// ② 부드럽게 보간(스윕 금지, 텔레포트)
	const FVector newLoc = FMath::VInterpTo(curr.GetLocation(), targetWS.GetLocation(), DeltaTime, posSpeed);
	const FQuat   newRot = FMath::QInterpTo(curr.GetRotation(), targetWS.GetRotation(), DeltaTime, rotSpeed);
	HeldComp->SetWorldTransform(FTransform(newRot, newLoc, curr.GetScale3D()),
		/*bSweep=*/false, nullptr, ETeleportType::TeleportPhysics);



	// ③ 손 IK 목표(월드 → AnimBP에서 컴포넌트로 변환)
	const FTransform R_WS = HeldComp->GetSocketTransform(RightSocketName, RTS_World);
	const FTransform L_WS = HeldComp->GetSocketTransform(LeftSocketName, RTS_World);
	AnimInstance->SetRightHandTarget(R_WS);
	AnimInstance->SetLeftHandTarget(L_WS);
}

void UInteractiveComponent::SetGripMidPoint(FName RSock, FName LSock)
{
	if (!HitResult.GetComponent() || !Character) return;

	UPrimitiveComponent* HeldComp = HitResult.GetComponent();
	RightSocketName = RSock;
	LeftSocketName = LSock;

	const FTransform R_CS = HeldComp->GetSocketTransform(RightSocketName, RTS_Component);
	const FTransform L_CS = HeldComp->GetSocketTransform(LeftSocketName, RTS_Component);

	const FVector Mid_CS = (R_CS.GetLocation() + L_CS.GetLocation()) * 0.5f;
	GripLocal = FTransform(FRotator::ZeroRotator, Mid_CS, FVector::OneVector); // **컴포넌트 공간**

	// 현재 오브젝트 위치를 캐리 프레임 기준으로 저장
	const FTransform carryWS = MakeCarryFrame();
	RelObjFromCarry = HeldComp->GetComponentTransform().GetRelativeTransform(carryWS);
}

