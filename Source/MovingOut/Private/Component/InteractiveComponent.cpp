
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
	Super::BeginPlay();
	Character = Cast<AMovingOutCharacter>(GetOwner());
	AnimInstance = Cast<UPlayerAnimInstance>(Character->GetMesh()->GetAnimInstance());
	if (Character) AddTickPrerequisiteActor(Character);
}

// ĳ�� ������(����): ��� ��ġ, Yaw=Actor Forward, Up=+Z
FTransform UInteractiveComponent::MakeCarryFrame()
{
	const FVector Hips = Character->GetMesh()->GetSocketLocation(TEXT("Hips"));
	const FVector fwd = Character->GetActorForwardVector();
	const FVector up = FVector::UpVector;
	const FQuat   rot = FRotationMatrix::MakeFromXZ(fwd, up).ToQuat();
	return FTransform(rot, Hips);
}

// ���� �̸����� �� �̱�
static FString LabelFrom(const FString& Name) {
	if (Name.Contains(TEXT("Forward"), ESearchCase::IgnoreCase)) return TEXT("Forward");
	if (Name.Contains(TEXT("Backward"), ESearchCase::IgnoreCase)) return TEXT("Backward");
	if (Name.Contains(TEXT("Left"), ESearchCase::IgnoreCase)) return TEXT("Left");
	if (Name.Contains(TEXT("Right"), ESearchCase::IgnoreCase)) return TEXT("Right");
	return TEXT("");
}

// �� �� �ش� ���� ���� ����
static FVector FaceNormalWS(const FString& Label, const FTransform& CompTM) {
	if (Label == "Forward")  return  CompTM.GetUnitAxis(EAxis::X);
	if (Label == "Backward") return -CompTM.GetUnitAxis(EAxis::X);
	if (Label == "Right")    return  CompTM.GetUnitAxis(EAxis::Y);
	if (Label == "Left")     return -CompTM.GetUnitAxis(EAxis::Y);
	return CompTM.GetUnitAxis(EAxis::X); // fallback
}

// N=�� ����, T=���� ����(+Z). �� ���� +X(�չٴ� ����) = -N, �� ���� +Y(�հ���) = T �� ����.
// �� ���̷��濡 ���� ���������� �̼����� ����.
static FQuat MakePalmFacingQuat(const FVector& N_ws, const FVector& T_ws, const FRotator& HandOffset)
{
	const FVector N = N_ws.GetSafeNormal();
	FVector T = (T_ws - FVector::DotProduct(T_ws, N) * N).GetSafeNormal(); // N�� ����ȭ

	// ���� +X(�չٴ�) = -N, ���� +Y(�հ���) = T
	const FQuat basisQ = FRotationMatrix::MakeFromXY(-N, T).ToQuat(); // �� ����!
	return basisQ * HandOffset.Quaternion();
}


void UInteractiveComponent::TryGrab()
{
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

				IsGrabbingSomething();
			
			}
			
		}
	}
}

void UInteractiveComponent::GrabRelease()
{
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
	
	if (HitResult.GetComponent())
	{
		HitResult.GetComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		HitResult.GetComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HitResult.GetComponent()->SetEnableGravity(true);
		HitResult.GetComponent()->SetSimulatePhysics(true);

		// �������� ��ǥ��
		const FVector Start = HitResult.GetComponent()->GetComponentLocation();
		const FVector Target = Character->CrosshairDecal->GetComponentLocation();
		const FVector ToTarget = Target - Start;

		// �����(XY)������ �Ÿ��� Yaw
		const double R   = FVector(ToTarget.X, ToTarget.Y, 0).Length();
		const double dZ  = ToTarget.Z;
		const double g   = -GetWorld()->GetGravityZ();      // ���

		// �װ� ���ϴ� �߻� ��ġ(���� ���� ����, +�� ����)
		const double PitchDeg = DesiredPitchDegrees;        // ��: 25.0
		const double theta    = FMath::DegreesToRadians(PitchDeg);

		const double cosT = FMath::Cos(theta);
		const double sinT = FMath::Sin(theta);
		const double tanT = FMath::Tan(theta);

		const double denom = (R * tanT - dZ) * 2.0 * cosT * cosT;

		if (R < KINDA_SMALL_NUMBER || denom <= 0.0 || cosT == 0.0)
		{
			// �� �����δ� �ذ� ����: ������ ���̰ų�(�� ������), �ٸ� ������� ���
			// ������ ����: ����ð� T ��� ���(���� �޽���)���� V0 ���
			const double T = FMath::Clamp(DesiredFlightTimeSeconds, 0.25, 3.0);
			const FVector V0 = (ToTarget / T) - 0.5 * FVector(0,0,-g) * T;
			HitResult.GetComponent()->SetPhysicsLinearVelocity(V0, true);
			return;
		}

		const double v2 = (g * R * R) / denom;
		const double v  = FMath::Sqrt(FMath::Max(v2, 0.0));

		// Yaw �� ��ǥ�� �ٶ󺸰�, Pitch �� �װ� ���� ������ ���
		const double YawDeg = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
		const FRotator LaunchRot(PitchDeg, YawDeg, 0.0);

		const FVector LaunchVel = LaunchRot.Vector() * v;   // cm/s
		HitResult.GetComponent()->SetPhysicsLinearVelocity(LaunchVel, true);
		
		/*
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
		*/
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

	// 1) �ʿ��� ���ϵ鸸 ������
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

	// �ּ��� �� �� ���� �־�� ��
	const bool HasXPair = SockF.IsSet() && SockB.IsSet();
	const bool HasYPair = SockL.IsSet() && SockR.IsSet();
	if (!HasXPair && !HasYPair) return false;

	// 2) �÷��̾ ��� "��"�� �� �������(���� ��ǥ�� ����)
	const FVector PlayerLS = CompTM.InverseTransformPosition(Character->GetActorLocation());

	// ��/��(��X)�� ��/��(��Y)������ �Ÿ�
	// ���ϵ��� ���� �߰����̸� min/max ��� �ܼ��� X/Y�� ���밪 �񱳷� ���
	const float dX = FMath::Abs(PlayerLS.X);
	const float dY = FMath::Abs(PlayerLS.Y);

	enum class EAxis { X, Y };
	EAxis UseAxis;
	if (!HasYPair) UseAxis = EAxis::X;
	else if (!HasXPair) UseAxis = EAxis::Y;
	else UseAxis = (dX >= dY) ? EAxis::Y : EAxis::X; // ��/�ڿ� �� ������ Y�� ��, ��/�쿡 �� ������ X�� ��

	// 3) ���� ����
	FTransform L_WS, R_WS;
	if (UseAxis == EAxis::Y) // Left/Right ��� ��� �� ����: L=Right, R=Left
	{
		if (!HasYPair) return false;
		L_WS = SockR.GetValue(); // �޼��� Right
		R_WS = SockL.GetValue(); // �������� Left
		SetGripMidPoint(FName("Edge_Left"), FName("Edge_Right"));
	}
	else // EAxis::X  �� Forward/Backward ��� ��� �� ����: L=Forward, R=Backward
	{
		if (!HasXPair) return false;
		L_WS = SockF.GetValue(); // �޼��� Forward
		R_WS = SockB.GetValue(); // �������� Backward
		SetGripMidPoint(FName("Edge_Backward"), FName("Edge_Forward"));
	}

	// 4) IK Ÿ�� ���� (AnimBP���� Component Space�� ��ȯ�ؼ� ����)
	AnimInstance->SetLeftHandTarget(L_WS);
	AnimInstance->SetRightHandTarget(R_WS);
	
	

    // (����) �����
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

	// 1) ��ǥ "�׸� ������" ��ġ(�÷��̾� ��)
	const FVector PelvisWS = Character->GetMesh()->GetBoneLocation(TEXT("Hips")); // �Ǵ� ĸ��/�޽� ��ġ
	const FVector Fwd = Character->GetActorForwardVector();
	const FVector Up = FVector::UpVector;

	FTransform DesiredMid_WS;
	DesiredMid_WS.SetLocation(PelvisWS + Fwd * S.CarryDist + Up * S.CarryHeight);

	// (����) ������Ʈ�� �÷��̾ ���ϰ�: Yaw�� ����
	const FRotator Face = FRotationMatrix::MakeFromXZ(-Fwd, Up).Rotator(); // ������ �÷��̾ ����
	DesiredMid_WS.SetRotation(Face.Quaternion());

	// 2) ������Ʈ �� Ʈ������ = ��ǥ �׸� * (���� �׸�)^-1
	const FTransform Curr = Comp->GetComponentTransform();
	const FTransform TargetWS = DesiredMid_WS * GripLocal.Inverse();

	// 3) �ε巴�� ���� + �ڷ���Ʈ ����(���� X)
	const FVector NewLoc = FMath::VInterpTo(Curr.GetLocation(), TargetWS.GetLocation(), DeltaTime, S.MovePosSpeed);
	const FQuat   NewRot = FMath::QInterpTo(Curr.GetRotation(), TargetWS.GetRotation(), DeltaTime, S.MoveRotSpeed);
	FTransform NewTM(NewRot, NewLoc, Curr.GetScale3D());

	Comp->SetWorldTransform(NewTM, /*bSweep=*/false, nullptr, ETeleportType::TeleportPhysics);

	// 4) �� IK ��ǥ ���� (ȸ�� ����)
	const FTransform R_SocketWS = Comp->GetSocketTransform(RightSocketName, RTS_World);
	const FTransform L_SocketWS = Comp->GetSocketTransform(LeftSocketName, RTS_World);

	// �� �� �� ����
	const FString RLabel = LabelFrom(RightSocketName.ToString());
	const FString LLabel = LabelFrom(LeftSocketName.ToString());
	const FVector N_R = FaceNormalWS(RLabel, CompTM);
	const FVector N_L = FaceNormalWS(LLabel, CompTM);

	// ���� ������ ����(+Z)�� ���� (���߿� Top/Bottom ������ ���� ���⼭ �б�)
	const FVector EdgeDir = CompTM.GetUnitAxis(EAxis::Z);

	// �� ���̷��� ����(�� ���� ���߸� ��)
	const FRotator HandOffset_R = FRotator(0, 90.f, 0.f);     // �ʿ�� �̼�����
	const FRotator HandOffset_L = FRotator(0, -90.f, 90.f);     // �޼��� ������ ���̸� Yaw 180 ��

	FTransform R_WS = R_SocketWS;
	FTransform L_WS = L_SocketWS;
	R_WS.SetRotation(MakePalmFacingQuat(N_R, EdgeDir, HandOffset_R));
	L_WS.SetRotation(MakePalmFacingQuat(N_L, EdgeDir, HandOffset_L));

	AnimInstance->SetRightHandTarget(R_WS);
	AnimInstance->SetLeftHandTarget(L_WS);

	// 5) �ִ� ��������: Light ���
	//AnimInstance->bUseLightCarryProfile = true;
}

void UInteractiveComponent::TickCarry_Heavy(float DeltaTime, const FCarrySettings& S)
{

}

void UInteractiveComponent::TickCarry_MoveCoupled(float DeltaTwime, float posSpeed, float rotSpeed)
{
	if (!HitResult.GetComponent()) return;
	UPrimitiveComponent* HeldComp = HitResult.GetComponent();

	
	
	/*
	// �� ��ǥ ������Ʈ ��ȯ = (�� ĳ��������) * (����� ���)
	const FTransform carryWS = MakeCarryFrame();
	const FTransform targetWS = RelObjFromCarry * carryWS; // �Ǵ� carryWS * RelObjFromCarry (UE ������ �°� Ȯ��)
	const FTransform curr = HeldComp->GetComponentTransform();

	// �� �ε巴�� ����(���� ����, �ڷ���Ʈ)
	const FVector newLoc = FMath::VInterpTo(curr.GetLocation(), targetWS.GetLocation(), DeltaTime, posSpeed);
	const FQuat   newRot = FMath::QInterpTo(curr.GetRotation(), targetWS.GetRotation(), DeltaTime, rotSpeed);
	HeldComp->SetWorldTransform(FTransform(newRot, newLoc, curr.GetScale3D()),false, nullptr, ETeleportType::TeleportPhysics);
	*/
	

	// �� �� IK ��ǥ(���� �� AnimBP���� ������Ʈ�� ��ȯ)
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

	HitResult.GetComponent()->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Character->GetRightHandBoneName());

	const FTransform R_CS = HeldComp->GetSocketTransform(RightSocketName, RTS_Component);
	const FTransform L_CS = HeldComp->GetSocketTransform(LeftSocketName, RTS_Component);

	const FVector Mid_CS = (R_CS.GetLocation() + L_CS.GetLocation()) * 0.5f;
	GripLocal = FTransform(FRotator::ZeroRotator, Mid_CS, FVector::OneVector); // **������Ʈ ����**

	// ���� ������Ʈ ��ġ�� ĳ�� ������ �������� ����
	const FTransform carryWS = MakeCarryFrame();
	RelObjFromCarry = HeldComp->GetComponentTransform().GetRelativeTransform(carryWS);
}

