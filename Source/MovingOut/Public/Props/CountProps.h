// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Props/PropsBase.h"
#include "CountProps.generated.h"

/**
 * PropsGoalZone에 카운트되는 props
 */
UCLASS()
class MOVINGOUT_API ACountProps : public APropsBase
{
	GENERATED_BODY()

public:
	ACountProps();

	virtual void PostInitializeComponents() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxCollision;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Change Material
	void ChangeMaterial(UMaterial* NewMaterial);
	// To default material
	void SetDefaultMaterial();
	
};
