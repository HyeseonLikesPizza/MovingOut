// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleWindow.generated.h"

class UGeometryCollectionComponent;
class UFieldSystemComponent;

UCLASS()
class MOVINGOUT_API ADestructibleWindow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleWindow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// Default mesh that will be shown before destruction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DefaultMesh;

	// GC that will actually be broken
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	// Trigger destruction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MasterFieldClass;

protected:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

public:
	FTimerHandle DestroyHandle;
	float WaitTime = 2.f;
};
