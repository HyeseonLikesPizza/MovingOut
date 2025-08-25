#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "AISplineCar.generated.h"

UCLASS()
class MOVINGOUT_API AAISplineCar : public APawn
{
	GENERATED_BODY()

public:
	AAISplineCar();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	class USplineComponent* TargetSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	float MaxSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	float SteeringForce = 2000000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	float ThrottleForce = 500000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CarMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* Spline;

private:
	float DistanceAlongSpline = 0.0f;
	FVector LastKnownSplineLocation;
    
	void FindSpline();
	void FollowSpline(float DeltaTime);
	void HandleCarPhysics();
	void HandleRecovery();
	bool IsCarOverturned();
};