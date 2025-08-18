// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PropsBase.generated.h"

/* 맵에 놓이는 가구와 가전 기본값 */
UCLASS()
class MOVINGOUT_API APropsBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APropsBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Default Static Mesh for props
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UMaterial* DefaultMaterial;
};
