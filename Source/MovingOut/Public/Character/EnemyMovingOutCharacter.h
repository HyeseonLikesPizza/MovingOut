// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "EnemyMovingOutCharacter.generated.h"


UCLASS()

class MOVINGOUT_API AEnemyMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()


	// 틱을 오버라이브 함
public:
	void Tick(float DeltaSeconds) override;
	
	// 비긴플레이 를 오버라이브 일부만 하용하게
protected:
	virtual void BeginPlay() override;
	
private:
	//플레이어 캐릭터 타겟팅
	AMovingOutCharacter* PlayerMovingOutCharacter;
	
	// 나의 회전 위치를 지정하기 위해
	FRotator TargetRotation;
	// 닿았을때 false 인 boolean
	bool Bhashit = false;
	// 돌았을떄 false 인 boolean
	bool bIsRotating;
	// 움직였을떄 false 인 boolean
	bool bIsMoving;
	
	// Forward 타이머 핸들
	FTimerHandle MovementTimerHandle;
	void EnemyMoveForward();
	
	// Rotation 타이머 핸들
	FTimerHandle RotationCooldownTimer;
	void ResetRotationCooldown();

	// 에너미에 힛 포인트를 불러온다
	UFUNCTION()
	void  OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// arrow 불러오기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow;
};
