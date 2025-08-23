

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
}


void AAISplineCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    if (TargetSpline)
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


// 스플라인을 따라 이동
void AAISplineCar::FollowSpline(float DeltaTime)
{
    
    const float SplineLength = TargetSpline->GetSplineLength();

    
    DistanceAlongSpline += MoveSpeed * DeltaTime;

    
    if (DistanceAlongSpline >= SplineLength)
    {
        
        DistanceAlongSpline = 0.0f;
    }

    // 현재 이동한 거리에 해당하는 스플라인 위의 위치와 방향
    const FVector NewLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
    const FRotator NewRotation = TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);


    // 메쉬에 힘을 계산
    
    const FVector CurrentLocation = GetActorLocation();
    const FVector TargetDirection = (NewLocation - CurrentLocation).GetSafeNormal();
    const FVector ForceToApply = TargetDirection * MoveSpeed * 100.0f; // 힘의 크기 조절

    // 목표 지점으로 이동(힘을 가해서)
    CarMesh->AddForce(ForceToApply, NAME_None, true); 

    // 회전은 항상 보간
    const FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 2.0f);
    SetActorRotation(InterpolatedRotation);
}
