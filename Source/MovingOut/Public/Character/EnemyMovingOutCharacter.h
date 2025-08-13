#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyMovingOutCharacter.generated.h"

// AI 상태를 정의하는 enum
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    ES_Idle UMETA(DisplayName = "Idle"),
    ES_Patrolling UMETA(DisplayName = "Patrolling"),
    ES_Chasing UMETA(DisplayName = "Chasing"),
    ES_HitReaction UMETA(DisplayName = "HitReaction"),
};

UCLASS()
class MOVINGOUT_API AEnemyMovingOutCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 생성자
    AEnemyMovingOutCharacter();

protected:
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:
    // 매 프레임 호출
    virtual void Tick(float DeltaSeconds) override;

    // AI 상태
    EEnemyState CurrentState;
    EEnemyState PreviousState;

    // AI 상태 변경 함수
    void SetEnemyState(EEnemyState NewState);

    // AI 행동 핸들러
    void HandleIdle();
    void HandlePatrolling(float DeltaTime);
    void HandleChasing(float DeltaTime);
    void HandleHitReaction(float DeltaTime);

    // 이벤트 함수
    UFUNCTION()
    void OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    UFUNCTION()
    void OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 패트롤 관련 함수
    void FindAndMoveToNewPatrolDestination();
    void EndHitReaction();
    void StartPatrolling();

private:
    // 플레이어 감지용 Sphere Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* PlayerDetectionSphere;

    // 현재 추적 대상
    UPROPERTY()
    class APlayerMovingOutCharacter* PlayerTarget;

    // 순찰 목표 지점
    FVector PatrolDestination;
    
    // 순찰 반경
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
    float PatrolRadius = 1500.f; // 기본값 설정

    // 충돌 반응 타이머
    FTimerHandle HitReactionTimer;

    // 충돌 반응 회전 목표
    FRotator TargetRotation;
};
