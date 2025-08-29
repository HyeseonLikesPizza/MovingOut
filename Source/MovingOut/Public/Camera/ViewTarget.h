// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ViewTarget.generated.h"

/**
 * 플레이어 폰/캐릭터의 카메라를 맵의 타겟 위치로 이동
 */
UCLASS()
class MOVINGOUT_API AViewTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViewTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
public:
	// Camera blend
	/** Target location of a camera move */
	UPROPERTY(EditAnywhere, Category = CameraBlend)
	AActor* TargetLocation;
	
	UPROPERTY(EditAnywhere, Category = CameraBlend)
	float BlendTime = 1.f;
	
	/** Triggered when the game signals that the camera start to move */
	UFUNCTION()
	void MoveToTargetLocation();
};
