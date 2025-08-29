// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropsGoalZone.generated.h"

/**
 * 플레이어가 물건을 갖다 놓는 목표지점
 * CountProps는 노란색으로 변함
 */
UCLASS()
class MOVINGOUT_API APropsGoalZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APropsGoalZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);

public:
	// Components
	/** Box collision that defines the area where props can be placed.  */ 
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DropZone;

	/** Material applied to the object when it's inside the placement zone. */
	UPROPERTY(VisibleAnywhere)
	UMaterial* Highlight;

protected:
	// Count props
	/** Total number of CountProps in DropZone */
	int32 TotalProps = 0;

public:
	FORCEINLINE int32 GetTotalProps() {return TotalProps;}
	FORCEINLINE void SetTotalProps(int32 InTotalProps) {TotalProps = InTotalProps;}
};
