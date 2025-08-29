
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

class AMovingOutCharacter;
class UPlayerAnimInstance;
class APlayerMovingOutCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOVINGOUT_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveComponent();

	bool ProjectToObjectSurface(UPrimitiveComponent* TargetComp, const FVector& Start,
	FVector& OutPoint, FVector& OutNormal);
	/* Grab, Throw */
	void TryGrab();
	void GrabRelease();
	void ThrowAim();
	void ThrowRelease();
	void SetThrowIndicatorVisible(bool bVisible);
	void CancelThrowAming();
	
	float ThrowAngle = 0.f;
	float ThrowSpeed = 30.f;
	float DesiredFlightTimeSeconds = 3.f;
	float DesiredPitchDegrees = 30.f;

	FORCEINLINE bool IsHoldingObject() const
	{
		if (CurrentGrabbedComp != nullptr) return true;
		else return false;
	}

	/* Tick */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Getter, Setter */
	FORCEINLINE bool GetIsAming() const { return IsAming; }

	// 플레이어 소켓
	UPROPERTY(EditAnywhere, Category="Grab|Hand")
	FName RightHandSocketName = TEXT("RightHand");
	
	UPROPERTY(EditAnywhere, Category="Grab|Hand")
	FName LeftHandSocketName  = TEXT("LeftHand");

	UPROPERTY(EditAnywhere, Category="Grab|Hand")
	FName PlayerFrontSocketName  = TEXT("");
	

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerMovingOutCharacter* Character;
	
	FHitResult HitResult;
	FVector AimPoint;
	bool IsAming;
	float CachedAimYaw;
	

	UPROPERTY()
	UPlayerAnimInstance* AnimInstance;

	FName LeftSocketName;
	FName RightSocketName;


public:
	// IK

	UPROPERTY()
	UPrimitiveComponent* CurrentGrabbedComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LeftHandIKTarget;

	UPROPERTY(EditDefaultsOnly, Category = "IK")
	float IKBlendInSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "IK")
	float IKBlendOutSpeed = 10.f;

	// AnimBP로 넘길 값들
	UPROPERTY(BlueprintReadOnly, Category="IK")
	FTransform LeftHandTarget_CS;   // 컴포넌트 공간
	UPROPERTY(BlueprintReadOnly, Category="IK")
	float LeftHandIKAlpha = 0.f;    // 0~1

private:
	bool SphereTrace(const FVector& Start, FHitResult& OutHit);
	FQuat RHtoObjDeltaQ;
};
