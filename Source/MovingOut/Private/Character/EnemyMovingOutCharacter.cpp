
#include "Character/EnemyMovingOutCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"

// 생성자
AEnemyMovingOutCharacter::AEnemyMovingOutCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 네비게이션 시스템을 쓰려면 반드시 필요!
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // 플레이어를 감지하기 위한 구체 컴포넌트 생성 및 설정
    PlayerDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectionSphere"));
    PlayerDetectionSphere->SetupAttachment(RootComponent);
    PlayerDetectionSphere->SetSphereRadius(1100.0f);
    PlayerDetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    
    // 플레이어 감지용 구체 컴포넌트에 이벤트 함수 바인딩
    PlayerDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerDetected);
    PlayerDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerLost);
   
    // 초기 상태 설정
    CurrentState = EEnemyState::ES_Idle;

    // 능력치 설정
    PatrolSpeed = 150.f;
    ChaseSpeed = 450.f;
    PatrolRadius = 2000.f;
    ObjectSearchRadius = 1500.f;
    ThrowForce = 1500.f;
    TargetObject = nullptr;
    GrabbedObject = nullptr;
}

// 게임 시작 시 호출
void AEnemyMovingOutCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // 캡슐 컴포넌트가 제대로 있는지 확인 후 충돌 이벤트 바인딩
    if (GetCapsuleComponent())
    {
       GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyMovingOutCharacter::OnEnemyHit);
    }
    
    // 게임 시작하고 1초 뒤에 순찰 시작
    FTimerHandle StartPatrolTimer;
    GetWorldTimerManager().SetTimer(StartPatrolTimer, this, &AEnemyMovingOutCharacter::StartPatrolling, 1.0f, false);
}

// 순찰 상태로 전환하는 함수
void AEnemyMovingOutCharacter::StartPatrolling()
{
    SetEnemyState(EEnemyState::ES_Patrolling);
}

// 매 프레임마다 호출
void AEnemyMovingOutCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 현재 상태에 따라 다른 행동을 함
    switch (CurrentState)
    {
    case EEnemyState::ES_Idle: HandleIdle(); break;
    case EEnemyState::ES_Patrolling: HandlePatrolling(DeltaTime); break;
    case EEnemyState::ES_Chasing: HandleChasing(DeltaTime); break;
    case EEnemyState::ES_HitReaction: HandleHitReaction(DeltaTime); break;
    case EEnemyState::ES_Grabbing: HandleGrabbing(DeltaTime); break;
    default: break; // 혹시 모를 상황에 대비
    }
}

// AI 상태를 변경하는 함수
void AEnemyMovingOutCharacter::SetEnemyState(EEnemyState NewState)
{
    if (CurrentState == NewState) return;
    
    // 충돌 반응 상태로 들어가기 전에 현재 상태를 저장해둠
    if (NewState == EEnemyState::ES_HitReaction)
    {
       PreviousState = CurrentState;
    }
    
    // 순찰 상태를 벗어날 때 타이머 초기화 (물건 잡기 시도)
    if (CurrentState == EEnemyState::ES_Patrolling)
    {
        GetWorldTimerManager().ClearTimer(ThrowAttemptTimer);
    }
    
    // 물건 잡기 상태를 벗어날 때 타겟 물건을 초기화함
    if (NewState != EEnemyState::ES_Grabbing)
    {
        TargetObject = nullptr;
    }

    CurrentState = NewState;
    UE_LOG(LogTemp, Warning, TEXT("AI State Changed to: %s"), *UEnum::GetValueAsString(NewState));

    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (!MovementComponent) return;

    // 새 상태에 맞춰 이동 속도와 타이머를 설정
    switch (NewState)
    {
        case EEnemyState::ES_Patrolling:
            MovementComponent->MaxWalkSpeed = PatrolSpeed;
            // 10초마다 물건을 잡으려 시도하는 타이머를 설정해둠
            GetWorldTimerManager().SetTimer(ThrowAttemptTimer, this, &AEnemyMovingOutCharacter::AttemptToGrabObject, 8.0f, true);
            FindAndMoveToNewPatrolDestination();
            break;
        case EEnemyState::ES_Chasing:
            MovementComponent->MaxWalkSpeed = ChaseSpeed;
            break;
        case EEnemyState::ES_Grabbing:
            MovementComponent->MaxWalkSpeed = PatrolSpeed;
            break;
        case EEnemyState::ES_Idle:
        case EEnemyState::ES_HitReaction:
        default:
            MovementComponent->MaxWalkSpeed = 0.f;
            break;
    }
}

void AEnemyMovingOutCharacter::HandleIdle()
{
    // 대기 상태에서는 할 게 없음!
}

void AEnemyMovingOutCharacter::HandlePatrolling(float DeltaTime)
{
    // 목표 지점에 거의 다 왔는지 확인
    const float DistanceToGoal = FVector::Dist(GetActorLocation(), PatrolDestination);
    if (DistanceToGoal < 140.f)
    {
       // 목표에 도착했으니, 새로운 순찰 지점을 찾아서 이동 시!
       FindAndMoveToNewPatrolDestination();
    }
}

void AEnemyMovingOutCharacter::HandleChasing(float DeltaTime)
{
    // 플레이어가 유효한지 확인
    if (PlayerTarget && PlayerTarget->IsValidLowLevel())
    {
       AAIController* AIController = Cast<AAIController>(GetController());
       if (AIController)
       {
           // AIController를 사용해 플레이어에게 이동
           UAIBlueprintHelperLibrary::SimpleMoveToActor(AIController, PlayerTarget);
       }
    }
    else
    {
       // 타겟이 사라지면 순찰 상태로 돌아감
       PlayerTarget = nullptr;
       SetEnemyState(EEnemyState::ES_Patrolling);
    }
}

// 물건을 잡기 위해 이동하고 잡는 로직
void AEnemyMovingOutCharacter::HandleGrabbing(float DeltaTime)
{
    if (!TargetObject)
    {
        // 목표가 사라지면 순찰 상태로 돌아가자
        SetEnemyState(EEnemyState::ES_Patrolling);
        return;
    }
    const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetObject->GetActorLocation());
    if (DistanceToTarget < 250.f)
    {
        // 목표에 충분히 가까워지면 이동을 멈춤
        GetController()->StopMovement();
        UPrimitiveComponent* ObjectMesh = Cast<UPrimitiveComponent>(TargetObject->GetRootComponent());
        if (ObjectMesh && ObjectMesh->IsSimulatingPhysics())
        {
            GrabbedObject = TargetObject;
            TargetObject = nullptr;
            // 물리 시뮬레이션을 끄고 캐릭터에 부착
            ObjectMesh->SetSimulatePhysics(false);
            GrabbedObject->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Hand_R"));
            
            // 물건을 잡고 잠시 대기 상태로 전환
            SetEnemyState(EEnemyState::ES_Idle);
            // 1.5초 뒤에 물건을 던지도록 타이머 설정
            GetWorldTimerManager().SetTimer(ThrowActionTimer, this, &AEnemyMovingOutCharacter::PerformThrow, 1.5f, false);
        }
        else
        {
            // 물건이 물리가 활성화된 상태가 아니면 잡기 실패! 순찰 상태로 돌아감
            TargetObject = nullptr;
            SetEnemyState(EEnemyState::ES_Patrolling);
        }
    }
}

void AEnemyMovingOutCharacter::HandleHitReaction(float DeltaTime)
{
    // 목표 회전값으로 부드럽게 돌기!
    FRotator CurrentRotation = GetActorRotation();
    FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
    SetActorRotation(InterpolatedRotation);
}


// 플레이어가 감지됐을 때 호출
void AEnemyMovingOutCharacter::OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 감지된 액터가 플레이어인지 확인!
    APlayerMovingOutCharacter* Player = Cast<APlayerMovingOutCharacter>(OtherActor);
    if (Player)
    {
       // 지금 벽에 부딪힌 상태가 아닐 때만 추적 상태로 변경
       if (CurrentState != EEnemyState::ES_HitReaction)
       {
           // 물건 잡으러 가는 중에 플레이어를 발견하면 잡기 시도를 중단
           if(CurrentState == EEnemyState::ES_Grabbing)
           {
               GetController()->StopMovement();
               TargetObject = nullptr;
           }
          PlayerTarget = Player;
          SetEnemyState(EEnemyState::ES_Chasing);
       }
    }
}

// 플레이어가 범위를 벗어났을 때 호출
void AEnemyMovingOutCharacter::OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 사라진 액터가 지금 추적 중인 플레이어인지 확인
    if (OtherActor == PlayerTarget)
    {
       PlayerTarget = nullptr;
       // 추적 중인 상태에서만 순찰 상태로 변경
       if(CurrentState == EEnemyState::ES_Chasing)
       {
          SetEnemyState(EEnemyState::ES_Patrolling);
          // 플레이어를 잃었으니 바로 새로운 순찰 지점으로 이동 시작
          FindAndMoveToNewPatrolDestination();
       }
    }
}

// 적이 무언가에 부딪혔을 때 호출
void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 충돌한 액터가 유효한지 확인하고, 자기 자신이 아닌지 확인
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    // 충돌한 액터가 플레이어 캐릭터인지 확인
    APlayerMovingOutCharacter* Player = Cast<APlayerMovingOutCharacter>(OtherActor);

    if (Player)
    {
        // 플레이어로부터 멀어지는 방향 벡터 계산
        FVector AwayFromPlayerDirection = (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();

        // 8만큼 밀어낼 위치 계산
        FVector PushbackLocation = GetActorLocation() + AwayFromPlayerDirection * 8.0f;

        // 즉시 해당 위치로 캐릭터 이동
        SetActorLocation(PushbackLocation, true);
        return;
    }

    // 충돌한 오브젝트가 Props 채널이 아닐 때만 반응함
    UPrimitiveComponent* OtherPrimComp = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
    if (OtherActor && OtherActor != this && OtherPrimComp && OtherPrimComp->GetCollisionObjectType() != COLLISION_CHANNEL_Probs)
    {
       // 진행 방향과 충돌 지점의 법선 벡터를 통해 정면 충돌인지 확인
       float ForwardDot = FVector::DotProduct(GetActorForwardVector(), -Hit.ImpactNormal);
       // 정면으로 부딪혔을 때만 반응
       if (ForwardDot > 0.5f)
       {
          AController* AIController = GetController();
          if (AIController)
          {
             AIController->StopMovement();
          }
          // 충돌 반응 상태로 전환함
          SetEnemyState(EEnemyState::ES_HitReaction);
          // 좌우로 랜덤하게 돔
          TargetRotation = GetActorRotation().Add(0.0f, FMath::RandBool() ? 110.f : -110.f, 0.0f);
          // 0.7초 후에 충돌 반응 상태를 끝내도록 타이머 설정
          GetWorldTimerManager().SetTimer(HitReactionTimer, this, &AEnemyMovingOutCharacter::EndHitReaction, 0.7f, false);
           
       }
    }
}

// 새로운 순찰 지점을 찾고 이동
void AEnemyMovingOutCharacter::FindAndMoveToNewPatrolDestination()
{
    if (CurrentState != EEnemyState::ES_Patrolling) return;
    
    AController* AIController = GetController();
    if (!AIController) return;
    
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return;

    FNavLocation RandomLocation;
    // 현재 위치 주변 PatrolRadius 반경 내에서 이동 가능한 지점을 찾음
    if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), PatrolRadius, RandomLocation))
    {
       PatrolDestination = RandomLocation.Location;
       UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolDestination);
    }
}

// 충돌 반응 상태 종료
void AEnemyMovingOutCharacter::EndHitReaction()
{
    GetWorldTimerManager().ClearTimer(HitReactionTimer);
    // 이전 상태로 다시 돌아가자
    SetEnemyState(PreviousState);
    // 만약 이전 상태가 순찰이었다면 새로운 길을 찾아서 다시 순찰 시작
    if (PreviousState == EEnemyState::ES_Patrolling)
    {
       FindAndMoveToNewPatrolDestination();
    }
}

// 주변에 던질 수 있는 물건을 찾고 잡으러 가는 함수
void AEnemyMovingOutCharacter::AttemptToGrabObject()
{
    if (CurrentState != EEnemyState::ES_Patrolling) return;
    
    TArray<AActor*> OverlappingActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    // Props 채널을 가진 오브젝트를 찾음
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_CHANNEL_Probs));
    
    bFound = false;
    
    if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), ObjectSearchRadius, ObjectTypes, AActor::StaticClass(), {}, OverlappingActors))
    {
        for (AActor* Actor : OverlappingActors)
        {
            UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
            // 물리 시뮬레이션 중인 오브젝트만 골라냄
            if (PrimComp && PrimComp->IsSimulatingPhysics())
            {
                bFound = true;
                TargetObject = Actor;
                SetEnemyState(EEnemyState::ES_Grabbing);
                UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), TargetObject);
                return; // 가장 먼저 찾은 오브젝트로 이동!
            }
        }
    }
    if (!bFound)
    {
        FindAndMoveToNewPatrolDestination();
    }
}

// 잡은 물건을 던지는 함수
void AEnemyMovingOutCharacter::PerformThrow()
{
    if (!GrabbedObject) return;
    
    // 플레이어를 향해 던지도록 방향을 계산함
    FVector ThrowDirection = (PlayerTarget ? (PlayerTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal() : GetActorForwardVector());

    UPrimitiveComponent* ObjectMesh = Cast<UPrimitiveComponent>(GrabbedObject->GetRootComponent());
    if (ObjectMesh)
    {   
        // 부착 해제
        GrabbedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        // 물리 시뮬레이션 다시 활성화
        ObjectMesh->SetSimulatePhysics(true);
        // 힘을 가해 던짐
        ObjectMesh->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true);
    }
    // 잡고 있던 물건 초기화
    GrabbedObject = nullptr;
    // 다시 순찰 상태로 돌아감
    SetEnemyState(EEnemyState::ES_Patrolling);
}