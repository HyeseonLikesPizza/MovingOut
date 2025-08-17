// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

class AMovingOutCharacter;
class UPlayerAnimInstance;

struct FCarrySettings {
	float CarryDist = 30.f; // 플레이어 앞 거리(캡슐 기준)
	float CarryHeight = 10.f; // 손/상자 높이 보정
	float MovePosSpeed = 5.f; // 위치 보간 속도
	float MoveRotSpeed = 1.f; // 회전 보간 속도
};

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
	bool PickFaceEdgesAndSetIK();

	FORCEINLINE bool GetIsAming() const { return IsAming; }
	FORCEINLINE void SetIsAming(const bool& InAming) { IsAming = InAming; }
	FORCEINLINE bool IsGrabbingSomething() 
	{ 
		if (HitResult.GetComponent()) return true;
		else return false;
	}

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TickCarry_Light(float DeltaTime, const FCarrySettings& S);
	void TickCarry_Heavy(float DeltaTime, const FCarrySettings& S);
	void SetGripMidPoint(FName RSock, FName LSock);

private:
	UPROPERTY()
	AMovingOutCharacter* Character;

	FHitResult HitResult;

	FVector AimPoint;

	bool IsAming;

	float CachedAimYaw;

	UPROPERTY()
	UPlayerAnimInstance* AnimInstance;

	FTransform GripLocal;
	FCarrySettings Settings;

	FName LeftSocketName;
	FName RightSocketName;
};
