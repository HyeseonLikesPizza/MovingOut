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
	AEnemyMovingOutCharacter();
	void Tick(float DeltaSeconds) override;
	//타겟을둠
	
	
	// 회전에 대한 float 변수들
	UPROPERTY(EditAnywhere)
	float RotationCurrentSpeed = 10.0f;

	UPROPERTY(EditAnywhere)
	float RotationMaxSpeed = 50.f;

	UPROPERTY(EditAnywhere)
	float RotationAcceleration = 5.0f;

	UPROPERTY(EditAnywhere)
	FRotator ETargetRotaion;


	
	// 비긴플레이 를 오버라이브 일부만 하용하게
protected:
	virtual void BeginPlay() override;
	
private:
	// 나의 회전 위치를 지정하기 위해
	FRotator TargetRotation;
	// 닿았을때 false 인 boolean
	bool Bhashit = false;
	// 돌았을떄 false 인 boolean
	bool bIsRotating;

	// 헤더에서 타이머 생성
	FTimerHandle RotationCooldownTimer;
	void ResetRotationCooldown();

	// 에너미에 힛 포인트를 불러온다
	UFUNCTION()
	void  OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// arrow 불러오기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow;
};