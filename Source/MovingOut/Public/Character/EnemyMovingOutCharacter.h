#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyMovingOutCharacter.generated.h"

#define COLLISION_CHANNEL_Probs ECC_GameTraceChannel1

// ai 상태 enum
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    ES_Idle UMETA(DisplayName = "Idle"),
    ES_Patrolling UMETA(DisplayName = "Patrolling"),
    ES_Chasing UMETA(DisplayName = "Chasing"),
    ES_Grabbing UMETA(DisplayName = "Grabbing"),
    ES_HitReaction UMETA(DisplayName = "HitReaction")
};

UCLASS()
class MOVINGOUT_API AEnemyMovingOutCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyMovingOutCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // sphere 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USphereComponent> PlayerDetectionSphere;

    // ai state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    EEnemyState CurrentState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI State")
    EEnemyState PreviousState;

    // ai 행동 제어 변수 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behavior")
    float PatrolSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behavior")
    float ChaseSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behavior")
    float PatrolRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behavior")
    float ObjectSearchRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Behavior")
    float ThrowForce;

    // 타겟 및 내부 변수
    UPROPERTY()
    TObjectPtr<class APlayerMovingOutCharacter> PlayerTarget;

    UPROPERTY()
    TObjectPtr<AActor> TargetObject;

    UPROPERTY()
    TObjectPtr<AActor> GrabbedObject;

    FVector PatrolDestination;
    FRotator TargetRotation;
    FTimerHandle HitReactionTimer;
    FTimerHandle ThrowAttemptTimer;
    FTimerHandle ThrowActionTimer;

    // 상태 관리 
    void SetEnemyState(EEnemyState NewState);

    // 상태별 핸들러 함수 
    void HandleIdle();
    void HandlePatrolling(float DeltaTime);
    void HandleChasing(float DeltaTime);
    void HandleGrabbing(float DeltaTime);
    void HandleHitReaction(float DeltaTime);

    // 이벤트 바인딩
    UFUNCTION()
    void OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 행동 관련 함수
    void StartPatrolling();
    void FindAndMoveToNewPatrolDestination();
    void EndHitReaction();
    void AttemptToGrabObject();
    void PerformThrow();
};
