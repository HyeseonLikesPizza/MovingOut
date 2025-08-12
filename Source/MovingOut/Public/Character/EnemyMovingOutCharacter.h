#pragma once
#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"                                                               
#include "Components/SphereComponent.h"
#include "NavigationSystem.h"
#include "EnemyMovingOutCharacter.generated.h"


// 에너미의 상태를 정의하는 열거형
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    ES_Idle UMETA(DisplayName = "Idle"),             // 대기 상태 (플레이어가 없을 때)
    ES_Patrolling UMETA(DisplayName = "Patrolling"), // 순찰 상태
    ES_Chasing UMETA(DisplayName = "Chasing"),       // 플레이어 추적 상태
    ES_HitReaction UMETA(DisplayName = "HitReaction")// 충돌 후 회전하는 상태
};

UCLASS()
class MOVINGOUT_API AEnemyMovingOutCharacter : public AMovingOutCharacter
{
    GENERATED_BODY()

public:
    AEnemyMovingOutCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaSeconds) override;

    bool IsOut = true;

private:
    // AI 상태를 설정하는 함수
    void SetEnemyState(EEnemyState NewState);
    
    // 현재 에너미의 상태
    EEnemyState CurrentState;
    // 피격 전 상태 저장
    EEnemyState PreviousState;

    // 플레이어 캐릭터 타겟
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    AMovingOutCharacter* PlayerMovingOutCharacter;

    // 에너미가 플레이어를 감지하는 구체 충돌 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USphereComponent* PlayerDetectionSphere;

    // 현재 순찰 지점
    FVector PatrolDestination;

    // 타겟 회전
    FRotator TargetRotation;

    // 회전 타이머 핸들
    FTimerHandle RotationCooldownTimer;
    void ResetRotationCooldown();

    // 캡슐 컴포넌트에 닿았을때 호출
    UFUNCTION()
    void OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 플레이어가 감지 범위에 들어왔을 때 호출
    UFUNCTION()
    void OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    // 각 상태별 로직을 처리하는 함수
    void HandlePatrolling(float DeltaTime);
    void HandleChasing(float DeltaTime);
    void HandleHitReaction(float DeltaTime);

    

    // 새로운 순찰 지점을 찾는 함수
    void FindNewPatrolDestination();
};
