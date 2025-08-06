// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Character/MovingOutCharacter.h"
#include "EnemyMovingOutCharacter.generated.h"


UCLASS()

class MOVINGOUT_API AEnemyMovingOutCharacter : public AMovingOutCharacter
{
	GENERATED_BODY()


	// 틱을 오버드라이브 함
public:
	AEnemyMovingOutCharacter();
	void Tick(float DeltaSeconds) override;

	// 비긴플레이 를 오버드라이브 일부만 하용하게
protected:
	virtual void BeginPlay() override;
	
private:

	// 에너미에 힛 포인트를 불러온다
	UFUNCTION()
	void OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// arrow 불러오기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow;
};