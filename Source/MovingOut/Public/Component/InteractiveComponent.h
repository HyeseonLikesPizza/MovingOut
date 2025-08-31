
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

class AMovingOutCharacter;
class UPlayerAnimInstance;
class APlayerMovingOutCharacter;

USTRUCT(BlueprintType)
struct FHoldingObjectData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> Component = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsHeavy = false;

	void Clear()
	{
		Component = nullptr;
		bIsHeavy = false;
	}

	bool IsEmpty() const
	{
		return Component.Get() == nullptr;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOVINGOUT_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveComponent();

	/* Tick */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, Category="Grab")
	FHoldingObjectData HoldingObjData; 
	
	/* Getter, Setter */
	FORCEINLINE bool GetIsAming() const { return IsAming; }

	
	/* Grab, Throw */
	void TryGrab();
	void GrabRelease();
	void ThrowAim();
	void ThrowRelease();
	void SetThrowIndicatorVisible(bool bVisible);
	void CancelThrowAming();
	bool SphereTrace(const FVector& Start, const FVector& End, FHitResult& OutHit);


protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	APlayerMovingOutCharacter* Character;

	
	/* Aim Variables */
	FVector AimPoint;
	bool IsAming;

	
	/* Throw Variables */
	float ThrowSpeed = 30.f;
	float DesiredFlightTimeSeconds = 3.f;
	float DesiredPitchDegrees = 30.f;
	
	
	/* Grab Variables */

	FVector LH_LocalPos;
	FRotator LH_LocalRot;
	FVector RH_LocalPos;
	FRotator RH_LocalRot;

	

	
};
