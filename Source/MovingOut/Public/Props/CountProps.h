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

public:
	// Change Material
	void ChangeMaterial(UMaterial* NewMaterial);
	// To default material
	void SetDefaultMaterial();
	
};
