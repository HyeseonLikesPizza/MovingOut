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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	AActor* SplineActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Car")
	float MoveSpeed = 3000.0f; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CarMesh; 

private:
	float DistanceAlongSpline = 0.0f;
	
	UPROPERTY()
	USplineComponent* TargetSplineComponent;
	
	// 함수
	void FollowSpline(float DeltaTime); 
	void InitializePhysics();
	void FindSpline();
};