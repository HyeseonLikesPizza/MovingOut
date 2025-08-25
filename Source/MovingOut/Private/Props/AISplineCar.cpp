#include "Props/AISplineCar.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AAISplineCar::AAISplineCar()
{
    PrimaryActorTick.bCanEverTick = true;
    CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
    RootComponent = CarMesh;
    CarMesh->SetSimulatePhysics(true);
    CarMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void AAISplineCar::BeginPlay()
{
    Super::BeginPlay();
    FindSpline();
}

void AAISplineCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!TargetSpline)
    {
        FindSpline();
        return;
    }
    
    FollowSpline(DeltaTime);
    HandleRecovery();
}

void AAISplineCar::FindSpline()
{
    if (TargetSpline) return;

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);
    
    for (AActor* Actor : FoundActors)
    {
        if (Actor->GetName().Equals(TEXT("BP_RoadSpline"), ESearchCase::IgnoreCase))
        {
            TargetSpline = Actor->FindComponentByClass<USplineComponent>();
            if (TargetSpline)
            {
                return;
            }
        }
    }
}

void AAISplineCar::FollowSpline(float DeltaTime)
{
    FVector CarLocation = GetActorLocation();
    
    // 스플라인 근접점 찾기
    float ClosestInputKey = TargetSpline->FindInputKeyClosestToWorldLocation(CarLocation);
    DistanceAlongSpline = TargetSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);
    
    // 50cm 앞을 목표로
    FVector ForwardPoint = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline + 50.0f, ESplineCoordinateSpace::World);
    
    FVector CurrentVelocity = CarMesh->GetPhysicsLinearVelocity();
    FVector TargetDirection = (ForwardPoint - CarLocation).GetSafeNormal();
    FVector CarForward = GetActorForwardVector();
    
    // 조향력 계산 (정확도 향상)
    FVector SteerTorque = GetActorUpVector() * FVector::CrossProduct(CarForward, TargetDirection).Z * SteeringForce;
    CarMesh->AddTorqueInDegrees(SteerTorque, NAME_None, true);
    
    // 추진력 계산 (속도 제어)
    float CurrentSpeed = FVector::DotProduct(CurrentVelocity, CarForward);
    float SpeedRatio = FMath::Clamp(1.0f - FMath::Abs(CurrentSpeed) / MaxSpeed, 0.0f, 1.0f);
    CarMesh->AddForce(CarForward * ThrottleForce * SpeedRatio, NAME_None, true);

    // 차체를 바닥에 붙이는 힘 추가
    FHitResult HitResult;
    FVector LineTraceStart = CarLocation;
    FVector LineTraceEnd = CarLocation - FVector::UpVector * 200.0f; // 200cm 아래
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, ECC_Visibility);
    
    if (bHit)
    {
        // 바닥과의 거리에 비례해 힘을 줘서 차를 바닥에 붙임
        float DistanceToGround = HitResult.Distance;
        float GroundForce = (150.0f - DistanceToGround) * 10000.0f; // 150cm 높이 유지
        CarMesh->AddForce(FVector::UpVector * GroundForce, NAME_None, true);
    }
    
    // 스플라인 이탈 시
    float DistFromSpline = FVector::Dist(CarLocation, TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World));
    if (DistFromSpline > 500.0f) // 5m 이상 이탈
    {
        FVector TowardsSpline = (TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World) - CarLocation).GetSafeNormal();
        CarMesh->AddForce(TowardsSpline * ThrottleForce, NAME_None, true);
    }
}

void AAISplineCar::HandleRecovery()
{
    FVector UpVector = GetActorUpVector();
    float UpDot = FVector::DotProduct(UpVector, FVector::UpVector);

    if (UpDot < 0.2f)
    {
        FRotator TargetRotation = GetActorRotation();
        TargetRotation.Roll = 0.0f;
        TargetRotation.Pitch = 0.0f;

        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
    }
}
bool AAISplineCar::IsCarOverturned()
{
    
    return false;
}