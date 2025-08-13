#pragma once

#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "Components/SphereComponent.h"
#include "EnemyMovingOutCharacter.generated.h" // 이 인클루드는 항상 마지막에 있어야 합니다.

// 에너미의 상태를 정의하는 열거형
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	ES_Idle UMETA(DisplayName = "Idle"),             // 대기 상태
	ES_Patrolling UMETA(DisplayName = "Patrolling"), // 순찰 상태
	ES_Chasing UMETA(DisplayName = "Chasing"),       // 플레이어 추적 상태
	ES_HitReaction UMETA(DisplayName = "HitReaction")// 피격 반응 상태
};

UCLASS()
class MOVINGOUT_API AEnemyMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()

public:
	AEnemyMovingOutCharacter();


	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI" , meta = (AllowPrivateAccess = "true") )
	float PatrolRadius = 1500.f;


protected:
	virtual void BeginPlay() override;
	void StartPatrolling();

private:
	// AI 상태 관련 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	EEnemyState CurrentState;

	// 피격 반응 전 상태를 저장하기 위한 변수
	UPROPERTY()
	EEnemyState PreviousState;

	// AI 상태를 설정하는 함수
	void SetEnemyState(EEnemyState NewState);

	// 각 상태별 로직을 처리하는 함수
	void HandleIdle();
	void HandlePatrolling(float DeltaTime);
	void HandleChasing(float DeltaTime);
	void HandleHitReaction(float DeltaTime);


	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* PlayerDetectionSphere;


	// 플레이어 감지 관련 
	// 추적할 플레이어 캐릭터
	UPROPERTY()
	class AMovingOutCharacter* PlayerTarget;

	// 플레이어가 감지 범위에 들어왔을 때 호출
	UFUNCTION()
	void OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 플레이어가 감지 범위를 벗어났을 때 호출
	UFUNCTION()
	void OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//** 순찰 관련 (Navigation System) **//
	// 현재 이동 목표 순찰 지점
	FVector PatrolDestination;
	
	// 순찰 반경 (에디터에서 수정 가능)
	

	// 새로운 순찰 지점을 찾고 이동을 시작하는 함수
	void FindAndMoveToNewPatrolDestination();


	// 피격 및 충돌 반응 관련 
	// 캡슐 컴포넌트에 닿았을때 호출
	UFUNCTION()
	void OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// 충돌 시 회전할 목표 각도
	FRotator TargetRotation;
	
	// 충돌 반응 상태를 끝내고 이전 상태로 복귀하는 타이머
	FTimerHandle HitReactionTimer;
	void EndHitReaction();

	
};