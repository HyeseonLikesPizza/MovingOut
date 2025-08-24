

#include "Props/AISplineCar.h"
#include "Components/StaticMeshComponent.h"




AAISplineCar::AAISplineCar()
{
    PrimaryActorTick.bCanEverTick = true;
    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    RootComponent = CarMesh;
}


void AAISplineCar::BeginPlay()
{
    Super::BeginPlay();
    InitializePhysics();
    FindSpline();
}


void AAISplineCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    if (TargetSplineComponent)
    {
        FollowSpline(DeltaTime);
    }
}

// 물리 시뮬레이션을 활성화하고 설정합니다.
void AAISplineCar::InitializePhysics()
{
    if (CarMesh)
    {
        // 물리 시뮬레이션을 활성화해야 충돌에 반응합니다.
        CarMesh->SetSimulatePhysics(true);
        // 무게 중심을 낮게 설정하여 안정성을 높입니다. (필요에 따라 조절)
        CarMesh->SetCenterOfMass(FVector(0, 0, -50.0f));
    }
}

void AAISplineCar::FindSpline()
{
    if (TargetSpline)
    {
        // 지정된 액터에서 SplineComponent를 찾습니다.
        TargetSplineComponent = TargetSpline;

        if (!TargetSplineComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("AISplineCar: SplineActor is set, but it has no SplineComponent!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AISplineCar: SplineActor is not set!"));
    }
}


// 스플라인을 따라 이동
void AAISplineCar::FollowSpline(float DeltaTime)
{
    // --- 1. 목표 지점 및 방향 설정 ---
    const float SplineLength = TargetSpline->GetSplineLength();
    
    // 이 부분은 약간의 예측을 위해 조금 더 앞을 보도록 수정합니다.
    // 현재 속도를 기반으로 다음 프레임에 어디쯤 가야 할지 예측하는 지점을 바라봅니다.
    FVector CurrentVelocity = CarMesh->GetPhysicsLinearVelocity();
    float LookAheadDistance = FMath::Clamp(CurrentVelocity.Size() * 0.2f, 100.0f, 1000.0f); // 0.2초 앞을 예측
    
    DistanceAlongSpline += CurrentVelocity.Size() * DeltaTime; // 실제 이동 속도 기반으로 거리 누적

    // 스플라인 끝에 도달하면 처음으로
    if (DistanceAlongSpline >= SplineLength)
    {
        DistanceAlongSpline = 0.0f;
    }
    
    // 우리가 가야 할 목표 위치와 회전값
    const FVector TargetLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline + LookAheadDistance, ESplineCoordinateSpace::World);
    const FRotator TargetRotation = TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

    
    // --- 2. 조향 (Steering) 로직: 목표 방향으로 토크(Torque) 가하기 ---
    
    // 현재 차의 회전값
    const FRotator CurrentRotation = GetActorRotation();
    // 목표 회전값까지 얼마나 차이가 나는지 계산 (Yaw 값만 사용)
    float SteerAngleDelta = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw);

    // 회전해야 할 방향으로 토크(회전력)를 가합니다.
    // Z축(UpVector)을 기준으로 회전시킵니다.
    FVector TorqueToApply = GetActorUpVector() * SteerAngleDelta * SteeringStiffness;
    CarMesh->AddTorqueInDegrees(TorqueToApply, NAME_None, true);


    // --- 3. 추진 (Throttle) 로직: 항상 차의 앞 방향으로 힘 가하기 ---
    
    // 현재 차의 앞쪽 방향
    const FVector ForwardDirection = GetActorForwardVector();
    // 앞쪽으로 추진력을 가합니다.
    CarMesh->AddForce(ForwardDirection * ThrottleForce, NAME_None, true);


    // --- 4. 미끄러짐 방지 (선택 사항이지만 추천) ---
    // 옆으로 흐르는 속도를 줄여주어 드리프트를 방지하고 안정적인 주행을 돕습니다.
    FVector SidewaysVelocity = FVector::DotProduct(CurrentVelocity, GetActorRightVector()) * GetActorRightVector();
    FVector DampingForce = -SidewaysVelocity * 2.0f; // 제동력 계수
    CarMesh->AddForce(DampingForce, NAME_None, true);
}
