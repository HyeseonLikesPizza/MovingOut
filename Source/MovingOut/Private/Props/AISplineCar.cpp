

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
    if (SplineActor)
    {
        // 지정된 액터에서 SplineComponent를 찾습니다.
        TargetSplineComponent = SplineActor->FindComponentByClass<USplineComponent>();

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
    const float SplineLength = TargetSplineComponent->GetSplineLength();

    DistanceAlongSpline += MoveSpeed * DeltaTime;
    
    if (DistanceAlongSpline >= SplineLength)
    {
        DistanceAlongSpline = 0.0f;
    }

    const FVector NewLocation = TargetSplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
    const FRotator NewRotation = TargetSplineComponent->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
    
    const FVector CurrentLocation = GetActorLocation();
    const FVector TargetDirection = (NewLocation - CurrentLocation).GetSafeNormal();
    const FVector ForceToApply = TargetDirection * MoveSpeed * 100.0f;

    CarMesh->AddForce(ForceToApply, NAME_None, true);

    const FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 2.0f);
    SetActorRotation(InterpolatedRotation);
}
