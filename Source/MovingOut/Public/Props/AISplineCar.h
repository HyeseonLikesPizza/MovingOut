#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AISplineCar.generated.h"

class USplineComponent;
class UStaticMeshComponent;

// 자동차의 상태를 정의하는 열거형
UENUM(BlueprintType)
enum class ECarState : uint8
{
	FollowingSpline, // 스플라인을 따라가는 정상 상태
	Derailed         // 충돌로 인해 스플라인을 벗어난 상태
};

UCLASS()
class MOVINGOUT_API AAISplineCar : public APawn
{
	GENERATED_BODY()

public:
	AAISplineCar();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 컴포넌트가 다른 객체와 부딪혔을 때 호출될 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// 상태에 따라 호출될 함수들
	void DriveAlongSpline(float DeltaTime);
	void ApplySuspensionForce();
	
public:
	//** 컴포넌트 **//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CarMesh;

	//** 스플라인 설정 **//
	// 따라갈 스플라인 컴포넌트를 가진 액터. 에디터에서 직접 지정해야 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Spline Settings")
	AActor* TargetSplineActor;

	//** 주행 파라미터 **//
	// 스플라인 위에서 얼마나 앞을 바라보고 주행할지 결정합니다 (cm 단위).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Movement")
	float LookAheadDistance = 400.0f;

	// 자동차의 최고 속도 (cm/s).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Movement")
	float MaxSpeed = 1000.0f;

	// 핸들링 강도. 높을수록 더 급격하게 회전합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Movement")
	float SteeringTorque = 1546.0f;

	// 주행 추진력.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Movement")
	float ThrottleForce = 1500.0f;

	//** 물리 및 충돌 **//
	// 경로를 이탈하게 만드는 최소 충격량. 이 값보다 큰 충격을 받으면 스플라인 추적을 멈춥니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Physics")
	float DerailmentImpulseThreshold = 1000.0f;
	
	//** 서스펜션 **//
	// 차체를 지면으로부터 띄우려는 힘.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Suspension")
	float SuspensionForce = 200000.0f;

	// 차체가 지면으로부터 유지하려는 목표 높이 (cm 단위).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car|Suspension")
	float SuspensionHeight = 80.0f;

private:
	// 참조할 스플라인 컴포넌트
	UPROPERTY()
	USplineComponent* TargetSpline;

	// 자동차의 현재 상태
	ECarState CurrentState = ECarState::FollowingSpline;

	// 스플라인 위에서의 현재 주행 거리
	float DistanceAlongSpline = 0.0f;
};