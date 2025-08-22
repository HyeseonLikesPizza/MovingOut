// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BendingTree.generated.h"

/*
 * 플레이어가 밀면 휘어지는 나무
 */
UCLASS()
class MOVINGOUT_API ABendingTree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABendingTree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TreeMesh;

protected:
	bool bIsBending = false;
	// Maximum bend angle of a tree mesh
	UPROPERTY(EditAnywhere)
	float MaxBendAngle = 20.f;

	// Maximum bend angle of a tree mesh
	UPROPERTY(EditAnywhere)
	float MinBendAngle = 5.f;
	
	// 속도 -> 회전각 변환 비율
	float AngleScale = 0.05;

	class APlayerMovingOutCharacter* CharacterRef;
};
