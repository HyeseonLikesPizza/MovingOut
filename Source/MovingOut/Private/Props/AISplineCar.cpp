#include "Props/AISplineCar.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

AAISplineCar::AAISplineCar()
{
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMesh;
	
	// 자동차는 물리 오브젝트로 작동해야 합니다.
	CarMesh->SetSimulatePhysics(true);
	// 보다 자동차에 적합한 콜리전 프로파일을 사용합니다 (프로젝트 설정에서 Vehicle 프로파일 확인 필요).
	CarMesh->SetCollisionProfileName(TEXT("Vehicle")); 
	CarMesh->SetMassOverrideInKg(NAME_None, 1500.0f); // 현실적인 무게 설정
}

void AAISplineCar::BeginPlay()
{
	Super::BeginPlay();

	// 에디터에서 설정한 TargetSplineActor로부터 SplineComponent를 찾아옵니다.
	if (TargetSplineActor)
	{
		TargetSpline = TargetSplineActor->FindComponentByClass<USplineComponent>();
	}

	// TargetSpline이 유효하지 않으면 경고를 로그에 남기고 이 액터의 Tick을 비활성화합니다.
	if (!TargetSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("AISplineCar '%s'에 유효한 TargetSpline이 지정되지 않았습니다. Tick을 비활성화합니다."), *GetName());
		SetActorTickEnabled(false);
		return;
	}

	// CarMesh의 OnComponentHit 이벤트에 OnHit 함수를 바인딩(연결)합니다.
	CarMesh->OnComponentHit.AddDynamic(this, &AAISplineCar::OnHit);
}

void AAISplineCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 상태에 따라 다른 로직을 실행합니다.
	switch (CurrentState)
	{
	case ECarState::FollowingSpline:
		DriveAlongSpline(DeltaTime);
		ApplySuspensionForce();
		break;

	case ECarState::Derailed:
		// 이 상태에서는 아무것도 하지 않고, 언리얼 물리 엔진이 모든 것을 처리하도록 둡니다.
		break;
	}
}

void AAISplineCar::DriveAlongSpline(float DeltaTime)
{
	if (!TargetSpline) return;

	const FVector CarLocation = GetActorLocation();
	const FVector CarForwardVector = GetActorForwardVector();

	// 자동차 위치에서 가장 가까운 스플라인 지점을 찾습니다.
	const float InputKey = TargetSpline->FindInputKeyClosestToWorldLocation(CarLocation);
	DistanceAlongSpline = TargetSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

	// 스플라인 위에서 일정 거리 앞을 목표 지점으로 설정합니다.
	const FVector TargetLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline + LookAheadDistance, ESplineCoordinateSpace::World);
	
	// --- 핸들링 (조향) ---
	const FVector DirectionToTarget = (TargetLocation - CarLocation).GetSafeNormal();
	// 외적(CrossProduct)을 이용해 차의 전방 벡터와 목표 방향 벡터 사이의 각도 차이를 계산합니다.
	const float SteeringValue = FVector::CrossProduct(CarForwardVector, DirectionToTarget).Z;
	const FVector TorqueToApply = GetActorUpVector() * SteeringValue * SteeringTorque;
	CarMesh->AddTorqueInDegrees(TorqueToApply, NAME_None, true);

	// --- 가속 (추진력) ---
	const FVector CurrentVelocity = CarMesh->GetPhysicsLinearVelocity();
	const float ForwardSpeed = FVector::DotProduct(CurrentVelocity, CarForwardVector);

	// 현재 속도가 최고 속도보다 낮을 때만 추진력을 가합니다.
	if (ForwardSpeed < MaxSpeed)
	{
		// 목표 방향과 현재 차의 방향이 일치할수록 더 강한 힘을 줍니다 (직선 주행 시 최대 가속).
		const float Alignment = FMath::Max(0.0f, FVector::DotProduct(CarForwardVector, DirectionToTarget));
		CarMesh->AddForce(CarForwardVector * ThrottleForce * Alignment, NAME_None, true);
	}
}

void AAISplineCar::ApplySuspensionForce()
{
	FHitResult HitResult;
	const FVector CarLocation = GetActorLocation();
	// 차체 아래로 레이를 쏴서 지면을 감지합니다.
	const FVector StartTrace = CarLocation;
	const FVector EndTrace = StartTrace - GetActorUpVector() * (SuspensionHeight * 1.5f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility))
	{
		// 현재 지면과의 거리를 측정합니다.
		const float CurrentSuspensionLength = HitResult.Distance;
		// 목표 높이와의 차이(압축된 정도)를 계산합니다.
		const float SuspensionCompression = SuspensionHeight - CurrentSuspensionLength;
		// 압축된 정도에 비례하여 위쪽으로 힘을 가합니다 (스프링 효과).
		const FVector ForceToApply = GetActorUpVector() * SuspensionCompression * SuspensionForce;

		CarMesh->AddForce(ForceToApply);
	}
}


void AAISplineCar::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 이미 경로를 이탈했거나, 자기 자신과 부딪힌 경우는 무시합니다.
	if (CurrentState == ECarState::Derailed || OtherActor == this)
	{
		return;
	}

	// 플레이어 폰과 부딪혔는지 확인합니다.
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor == PlayerPawn)
	{
		// 충격량이 설정된 임계값보다 크면 상태를 변경합니다.
		if (NormalImpulse.Size() > DerailmentImpulseThreshold)
		{
			CurrentState = ECarState::Derailed;
			UE_LOG(LogTemp, Log, TEXT("AISplineCar '%s'가 플레이어에 의해 경로를 이탈했습니다!"), *GetName());
		}
	}
}
