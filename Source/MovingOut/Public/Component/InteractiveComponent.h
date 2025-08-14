// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

class AMovingOutCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOVINGOUT_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveComponent();
	void TryGrab();
	void GrabRelease();
	void ThrowAim();
	void ThrowRelease();
	void SetThrowIndicatorVisible(bool bVisible);
	void CancelThrowAming();

	FORCEINLINE bool GetIsAming() const { return IsAming; }
	FORCEINLINE void SetIsAming(const bool& InAming) { IsAming = InAming; }

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	AMovingOutCharacter* Character;

	FHitResult HitResult;

	FVector AimPoint;

	bool IsAming;

	float CachedAimYaw;
};
