// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MovingOutCharacter.generated.h"

UCLASS(Blueprintable)
class AMovingOutCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMovingOutCharacter();
	
	virtual void Tick(float DeltaSeconds) override;
	
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void Walk();

	FORCEINLINE float GetGrabTraceDistance() const { return GrabTraceDistance; }
	FORCEINLINE float GetGrabDistance() const { return GrabDistance; }
	FORCEINLINE FName GetRightHandBoneName() const { return RightHandBoneName; }
	FORCEINLINE FName GetLeftHandBoneName() const { return LeftHandBoneName; }
	FORCEINLINE bool GetIsGrabbing() const { return bIsGrabbing; }

	FORCEINLINE void SetGrabTraceDistance(const float& InDistance) { GrabTraceDistance = InDistance; }
	FORCEINLINE void SetGrabDistance(const float& InDistance) { GrabDistance = InDistance; }
	FORCEINLINE void SetIsGrabbing(const bool& InGrabbing) { bIsGrabbing = InGrabbing; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LightCone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDecalComponent* CrosshairDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseSpeed = 300.f;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grab)
	float GrabTraceDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grab)
	float GrabDistance;

	UPROPERTY(EditDefaultsOnly, Category = Grab)
	FName LeftHandBoneName;

	UPROPERTY(EditDefaultsOnly, Category = Grab)
	FName RightHandBoneName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGrabbing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	
	


private:


	
};

