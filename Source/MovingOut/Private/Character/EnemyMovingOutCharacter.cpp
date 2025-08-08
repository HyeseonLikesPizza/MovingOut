// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyMovingOutCharacter.h"
 // #include "Components/ArrowComponent.h"
#include "Character/MovingOutCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
 // #include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include  "NavigationSystem.h"
 // #include "../DebugHelpers.h"
 



// 캡술컴포넌트에 닿았을때 인지 없다면 에러뜨고 없다고 뜨기
void AEnemyMovingOutCharacter::BeginPlay()
{
	
	Super::BeginPlay();

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	PlayerMovingOutCharacter = Cast<AMovingOutCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	
	if(GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyMovingOutCharacter::OnEnemyHit); // 캡슐 콜리전 이 내몸에 닿을때 Add Dynamic
		GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		UE_LOG(LogTemp, Log , TEXT("Enemy에 캡슐 컴포넌트 존재 확인"));
	}
	else
	{	// 차후에 destroy 될일 있다면 없다고 뜰 예정 근데이거 tick 옮겨야 하는거 아닌가 
		UE_LOG(LogTemp, Error, TEXT("Enemy에 캡슐 컴포넌트가 없습니다!"));
	}
}
void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy가 %s와 충돌했습니다!"), *OtherActor->GetName());
	
	if (Bhashit)
	{
		return;
	}

	if (OtherActor && OtherActor != this)
	{
		FVector ImpactDirection = Hit.ImpactPoint - GetActorLocation();
		ImpactDirection.Normalize();  //노말라이즈 시켜

		FVector ForwardVector = GetActorForwardVector();
		FVector CrossProduct = FVector::CrossProduct(ForwardVector, ImpactDirection);

		FRotator CurrentRotation = GetActorRotation();
		if (CrossProduct.Z > 0)
		{
			TargetRotation = CurrentRotation.Add(0.0f, -45.0f, 0.0f); // 왼쪽으로 회전
		}
		else
		{
			TargetRotation = CurrentRotation.Add(0.0f, 45.0f, 0.0f); // 오른쪽으로 회전
		}

		// 회전을 시작하고 이동을 멈추도록 플래그를 설정합니다.
		bIsRotating = true;
		bIsMoving = false; // 충돌 즉시 앞으로 이동을 멈춥니다.

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("회전!"));
		}

		Bhashit = true;   // 타이머는 추후 수정 예정
		GetWorld()->GetTimerManager().SetTimer(RotationCooldownTimer, this, &AEnemyMovingOutCharacter::ResetRotationCooldown, 0.65f, false);
	}
}


// 다시 처음으로 돌아가게
void AEnemyMovingOutCharacter::ResetRotationCooldown()
{
	Bhashit = false;
}
// 위에까지 OnHit event



// 델타 타임 줄떄마다 계속 활동
void AEnemyMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	// 회전 중일 때
	if (bIsRotating)
	{
		// RInterpTo 함수를 사용하여 현재 회전 값에서 목표 회전 값으로 부드럽게
		// InterpSpeed를 조절하여 회전 속도를 변경
		FRotator CurrentRotation = GetActorRotation();
		float InterpSpeed = 8.0f; 

		FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, InterpSpeed);
		SetActorRotation(InterpolatedRotation);

		// 목표 회전 값에 거의 도달했는지 확인
		if (FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetRotation.Yaw, 1.0f))
		{
			bIsRotating = false; // 회전이 끝나면 회전 플래그를 false로
			bIsMoving = true; // 앞으로 다시 이동 시작
		}
		
		// 회전 중일 때만 이동
		if (bIsMoving)
		{
			return;	
		};
		FVector Direction = GetActorForwardVector();
		// 캐릭터를 전방 방향으로 이동
		AddMovementInput(Direction, 1.0f);
		bIsRotating = true;
		bIsMoving = false;

	}
	else if (PlayerMovingOutCharacter)  //PlayerMovingChatacter 추적
	{
		//playerMovingOutCharacter 의 lcoation 값을 가져옴
		FVector PlayerLocation = PlayerMovingOutCharacter->GetActorLocation();
		FVector EnemyLocation = GetActorLocation();

		// 플레이어를 향하는 방향 벡터를 계산 + normalize
		FVector DirectionToPlayer = PlayerLocation - EnemyLocation;
		DirectionToPlayer.Normalize();

		// 방향 벡터를 FRotator로 변환하여 목표 회전 값으로 사용
		FRotator TargetPlayerRotation = DirectionToPlayer.Rotation();
    
		FRotator CurrentRotation = GetActorRotation();
		float InterpSpeed = 10.0f; // 회전 속도

		// 현재 회전을 목표 회전 값으로 부드럽게 보간
		FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetPlayerRotation, DeltaSeconds, InterpSpeed);
		SetActorRotation(InterpolatedRotation);

		// 보간된 회전 방향으로 이동하는 것이 아닌,
		// 처음 계산한 방향 벡터로 이동
		AddMovementInput(DirectionToPlayer, 1.0f);
	}
	// 플레이어가 없거나 회전 중이 아닐 때 기존 이동 로직
	else
	{
		FVector Direction = GetActorForwardVector();
		AddMovementInput(Direction, 1.0f);
	}


	
};		

