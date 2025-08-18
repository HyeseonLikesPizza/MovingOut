
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"

class AMovingOutCharacter;
class UPlayerAnimInstance;

struct FCarrySettings {
	float CarryDist = 30.f; // �÷��̾� �� �Ÿ�(ĸ�� ����)
	float CarryHeight = 10.f; // ��/���� ���� ����
	float MovePosSpeed = 5.f; // ��ġ ���� �ӵ�
	float MoveRotSpeed = 1.f; // ȸ�� ���� �ӵ�
	float PosSpeed = 12.f;
	float RotSpeed = 10.f;
	float PushOut = 4.f; // �� ������ ��¦ ����
	float ForwardAhead = 18.f; // �÷��̾� ��������
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOVINGOUT_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveComponent();

	/* Grab, Throw */
	void TryGrab();
	void GrabRelease();
	void ThrowAim();
	void ThrowRelease();
	void SetThrowIndicatorVisible(bool bVisible);
	void CancelThrowAming();
	bool PickFaceEdgesAndSetIK();
	void SetGripMidPoint(FName RSock, FName LSock);
	FTransform MakeCarryFrame();
	
	float ThrowAngle = 30.f;
	float ThrowSpeed = 100.f;

	/* Tick */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TickCarry_Light(float DeltaTime, const FCarrySettings& S);
	void TickCarry_Heavy(float DeltaTime, const FCarrySettings& S);
	void TickCarry_MoveCoupled(float DeltaTime, float posSpeed = 12.f, float rotSpeed = 10.f);

	/* Getter, Setter */
	FORCEINLINE bool GetIsAming() const { return IsAming; }
	FORCEINLINE void SetIsAming(const bool& InAming) { IsAming = InAming; }
	FORCEINLINE bool IsGrabbingSomething() 
	{ 
		if (HitResult.GetComponent()) return true;
		else return false;
	}

protected:
	virtual void BeginPlay() override;

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
	FTransform RelObjFromCarry;

	FName LeftSocketName;
	FName RightSocketName;
};
