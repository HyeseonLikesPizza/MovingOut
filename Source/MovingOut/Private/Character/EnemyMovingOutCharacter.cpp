

#include "Character/EnemyMovingOutCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
 // #include "Kismet/GameplayStatics.h"
 // #include "GameFramework/CharacterMovementComponent.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

// 생성자
AEnemyMovingOutCharacter::AEnemyMovingOutCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 플레이어 감지용 SphereComponent 생성 및 설정
    PlayerDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectionSphere"));
    PlayerDetectionSphere->SetupAttachment(RootComponent);
    PlayerDetectionSphere->SetSphereRadius(500.f);
    PlayerDetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    PlayerDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerDetected);

    CurrentState = EEnemyState::ES_Idle;
}

// 게임 시작 시 호출
void AEnemyMovingOutCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 캡슐 컴포넌트가 있는지 확인 후 충돌 이벤트 바인딩
    if(GetCapsuleComponent())
    {
       GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyMovingOutCharacter::OnEnemyHit);
       GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
       GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
       UE_LOG(LogTemp, Log, TEXT("Enemy에 캡슐 컴포넌트 존재 확인"));
    }
    else
    {
       UE_LOG(LogTemp, Error, TEXT("Enemy에 캡슐 컴포넌트가 없습니다!"));
    }
    
    // 게임 시작 시 순찰 상태로 시작
    PlayerDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerLost);

    SetEnemyState(EEnemyState::ES_Patrolling);
}

// 매 프레임 호출
void AEnemyMovingOutCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 현재 상태에 따라 다른 로직을 실행
    switch (CurrentState)
    {
            // 대기 상태에서는 아무것도 하지 않음
        case EEnemyState::ES_Idle:
            
            break;
            // 순찰 상태 계속 움직임
        case EEnemyState::ES_Patrolling:
            HandlePatrolling(DeltaSeconds);
            break;
            // raduis 에 hit 되면 추적
        case EEnemyState::ES_Chasing:
            HandleChasing(DeltaSeconds);
            break;
            //hit 되면 회전
        case EEnemyState::ES_HitReaction:
            HandleHitReaction(DeltaSeconds);
            break;
    }
}

// 충돌 이벤트 핸들러 (공사예정)

void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    
    UE_LOG(LogTemp, Warning, TEXT("Enemy가 %s와 충돌했습니다!"), *OtherActor->GetName());

    if (OtherActor && OtherActor != this)
    {
        SetEnemyState(EEnemyState::ES_HitReaction);

        FVector ImpactDirection = Hit.ImpactPoint - GetActorLocation();
        ImpactDirection.Normalize();

        FVector ForwardVector = GetActorForwardVector();
        FVector CrossProduct = FVector::CrossProduct(ForwardVector, ImpactDirection);

        
        FRotator CurrentRotation = GetActorRotation();
        if (CrossProduct.Z > 0)
        {
            TargetRotation = CurrentRotation.Add(0.0f, -75.0f, 0.0f); // 왼쪽으로 회전
        }
        else
        {
            TargetRotation = CurrentRotation.Add(0.0f, 75.0f, 0.0f); // 오른쪽으로 회전
        }
        

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("회전!"));
        }

        // 회전 후 추격/순찰 상태로 돌아가도록 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(RotationCooldownTimer, this, &AEnemyMovingOutCharacter::ResetRotationCooldown, 0.2f, false);
    }
}


// 플레이어 감지 이벤트 핸들러
void AEnemyMovingOutCharacter::OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(TEXT("player")))
    {
        if (CurrentState != EEnemyState::ES_Chasing && CurrentState != EEnemyState::ES_HitReaction)
        {
            //플레이어 캐릭터 캐스팅 후 스테이트 변경
            PlayerMovingOutCharacter = Cast<AMovingOutCharacter>(OtherActor);
            SetEnemyState(EEnemyState::ES_Chasing);
        }
    }
}

// 회전 후 상태 복구
void AEnemyMovingOutCharacter::ResetRotationCooldown()
{
    if (PlayerMovingOutCharacter && PlayerMovingOutCharacter->IsValidLowLevel())
    {
        SetEnemyState(EEnemyState::ES_Chasing);
    }
    else
    {
        SetEnemyState(EEnemyState::ES_Patrolling);
    }
}

// AI 상태 변경 함수
void AEnemyMovingOutCharacter::SetEnemyState(EEnemyState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    UE_LOG(LogTemp, Warning, TEXT("AI State Changed to: %s"), *UEnum::GetValueAsString(NewState));

    // 상태 전환 시 필요한 초기화
    if (NewState == EEnemyState::ES_Patrolling)
    {
        // 아래는 순찰 로직 
        PatrolDestination = GetActorLocation() + FVector(FMath::RandRange(-5000.f, 5000.f),
            FMath::RandRange(-5000.f, 5000.f), 0.f);
    }
}

void AEnemyMovingOutCharacter::OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(TEXT("player")))
    {
        // 플레이어 참조 제거
        PlayerMovingOutCharacter = nullptr;

        // 현재 상태가 추적 중일 때만 순찰 상태로 변경
        if (CurrentState == EEnemyState::ES_Chasing)
        {
            PlayerMovingOutCharacter = Cast<AMovingOutCharacter>(OtherActor);
            SetEnemyState(EEnemyState::ES_Chasing);
            UE_LOG(LogTemp, Warning, TEXT("플레이어 감지 범위를 벗어남 → 순찰 상태로 변경"));
        
        }
    }
}

// 새로운 순찰 지점 찾기
void AEnemyMovingOutCharacter::FindNewPatrolDestination()
{
    PatrolDestination = GetActorLocation() + FVector(FMath::RandRange(-5000.f, 5000.f), FMath::RandRange(-5000.f, 5000.f), 0.f);
}

// 순찰 상태 로직 
void AEnemyMovingOutCharacter::HandlePatrolling(float DeltaTime)
{
    // 순찰 지점에 거의 도달했는지 확인
    if (FVector::Dist(GetActorLocation(), PatrolDestination) < 450.f)
    {
        FindNewPatrolDestination();
    }

    // 순찰 지점을 향해 직선으로 이동
    FVector Direction = (PatrolDestination - GetActorLocation());
    Direction.Z = 0.0f; // Z축 이동을 막음
    Direction.Normalize();
    
    AddMovementInput(Direction, 0.4f); // 순찰 속도는 추격보다 느리게 설정
    
    // 이동 방향으로 회전
    FRotator NewTargetRotation = Direction.Rotation();
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), NewTargetRotation, DeltaTime, 15.0f); // 순찰 중에는 느리게 회전
    SetActorRotation(NewRotation);
}


// 추격 상태 로직 
void AEnemyMovingOutCharacter::HandleChasing(float DeltaTime)
{
    
    if (PlayerMovingOutCharacter && PlayerMovingOutCharacter->IsValidLowLevel())
    {
        FVector PlayerLocation = PlayerMovingOutCharacter->GetActorLocation();
        
        //  현재 위치에서 플레이어 위치까지의 방향 벡터를 계산합니다.
        FVector DirectionToPlayer = PlayerLocation - GetActorLocation();
        DirectionToPlayer.Z = 0.0f; // 날라가는거 방지 normalize
        DirectionToPlayer.Normalize();

        //  계산된 방향으로 이동 입력을 추가합니다.
        AddMovementInput(DirectionToPlayer, 0.8f);
        
        // 캐릭터가 이동 방향을 바라보도록 회전 로직을 추가합니다.
        FRotator NewTargetRotation = DirectionToPlayer.Rotation();
        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), NewTargetRotation, DeltaTime, 50.0f);
        SetActorRotation(NewRotation);
    }
    else
    {
        
        // 타겟을 잃으면 순찰 상태로 돌아갑니다.
        PlayerMovingOutCharacter = nullptr;
        SetEnemyState(EEnemyState::ES_Patrolling);
        
        
    }
}




// 피격 반응 상태 로직
void AEnemyMovingOutCharacter::HandleHitReaction(float DeltaTime)
{
    FRotator CurrentRotation = GetActorRotation();
    float InterpSpeed = 12.0f; 

    FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
    SetActorRotation(InterpolatedRotation);

    FVector Direction = (PatrolDestination - GetActorLocation());
    Direction.Z = 0.0f; // Z축 이동을 막음
    Direction.Normalize();

}
