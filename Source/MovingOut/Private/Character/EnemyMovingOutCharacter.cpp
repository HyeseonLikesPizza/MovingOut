// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyMovingOutCharacter.h"
#include "Components/ArrowComponent.h"
#include "Character/MovingOutCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../DebugHelpers.h"

 

// 캡술컴포넌트에 닿았을때 인지 없다면 에러뜨고 없다고 뜨기
void AEnemyMovingOutCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(GetCapsuleComponent())
	{ // 캡슐 컨포넌트에 add dynamic 추가 - 온 히트 할 시에 밑에 시뮬레이션 피직스 닿을떄 리깅 닿아져 있는 내 몸에 닿을떄
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyMovingOutCharacter::OnEnemyHit);
		GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		// 존재 한다면 존재 한다는 텍스트 띄움	
		UE_LOG(LogTemp, Log , TEXT("Enemy에 캡슐 컴포넌트 존재 확인")); 
	}
	else
	{	// 차후에 destroy 될일 있다면 없다고 뜰 예정 근데이거 tick 옮겨야 하는거 아닌가 
		UE_LOG(LogTemp, Error, TEXT("Enemy에 캡슐 컴포넌트가 없습니다!"));
	}
}

void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 충돌 로그 찍기
	UE_LOG(LogTemp, Warning, TEXT("Enemy가 %s와 충돌했습니다!"), *OtherActor->GetName());

	//이미 Hit 처리 중이면 리턴
	if (Bhashit)
	{
		return;
	}

	if (OtherActor && OtherActor != this)
	{
		// 충돌한 지점과 나의 위치로부터 충돌 방향 벡터 계산
		FVector ImpactDirection = Hit.ImpactPoint - GetActorLocation();
		ImpactDirection.Normalize();

		//내 현재 앞방향 벡터
		FVector ForwardVector = GetActorForwardVector();

		//외적값을 계산
		FVector CrossProduct = FVector::CrossProduct(ForwardVector, ImpactDirection);

		//내 회전 값 가져오기
		FRotator NewRotation = GetActorRotation();

		//  외적의 Z값을 보고 어느 방향으로 부딪혔는지 판별
		FRotator CurrentRotation = GetActorRotation();
		if (CrossProduct.Z > 0)
		{
			// 오른쪽에 부딪혔으니 왼쪽으로 튼다
			TargetRotation = CurrentRotation.Add(0.0f, -75.0f, 0.0f);
		}
		else
		{
			// 왼쪽에 부딪혔으니 오른쪽으로 튼다
			TargetRotation = CurrentRotation.Add(0.0f, 75.0f, 0.0f);
		}

		// 회전 시작 상태로 설정
		bIsRotating = true;

		// 화면 출력 (디버그)
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("회전!"));
		}

		// 쿨타임 시작 쿨타임은 n.f 값만큼 넣음
		Bhashit = true;
		GetWorld()->GetTimerManager().SetTimer(RotationCooldownTimer, this, &AEnemyMovingOutCharacter::ResetRotationCooldown, 1.0f, false);
		
		
	}
	
}
// 다시 처음으로 돌아가게
void AEnemyMovingOutCharacter::ResetRotationCooldown()
{
	Bhashit = false;
}
// 위에까지 OnHit event


// 애로우 컴포넌트 추가 밑에서 변수 넣으면 활용가능
AEnemyMovingOutCharacter::AEnemyMovingOutCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	if(Arrow)
	{
		Arrow->SetupAttachment(RootComponent);
	}
}
// 델타 타임 줄떄마다 계속 활동
void AEnemyMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRotating)
	{
		FRotator CurrentRotation = GetActorRotation();
		float InterpSpeed = 7.0f; 

		FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, InterpSpeed);
		SetActorRotation(InterpolatedRotation);

		// 목표 회전에 거의 도달했는지 확인
		
		if (FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetRotation.Yaw, 1.0f))
		{
			bIsRotating = false; // 회전이 끝나면 플래그를 false로
		}
		
	}
	// 틱마다 fVector x 축으로 쭉 간다
	FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, 1.0f);
	
	// 라인 트레이서 DrawDebugLineTrace(GetWorld(),start ,end,)
	// 트레이서  히트 포인트 DrawDebugHitPoint(GetWorld(),FHitResult,)
}	



