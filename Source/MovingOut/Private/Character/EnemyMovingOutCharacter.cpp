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

// 에너미 캐릭터가 hit 돼고 로고 text 띄어지게 만들어줌

	void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy가 %s와 충돌했습니다!"), *OtherActor->GetName());
	
		// boolean 불러옴
		if (Bhashit)
		{
			return;
		
		}
		if (OtherActor && OtherActor != this)
		{
			

			// 현재 Rotation 가져오기 n'도로 회전
			FRotator TargetRotation = GetActorRotation();
			TargetRotation.Yaw -= 75.f;
			FRotator CurrentRotation = GetActorRotation();
			float DeltaTime = GetWorld()->GetDeltaSeconds();  //월드에서 시간 가져오고 fps
			float InterpSpeed = 17.5f;  // 값이 높을수록 빠르게 회전

			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
			SetActorRotation(NewRotation);

			

			// 게임 화면에 출력 (Debug)
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("회전!"));
			}
		}
		//n초짜리 타이머 생성
		Bhashit = true;
		GetWorld()->GetTimerManager().SetTimer(RotationCooldownTimer, this, &AEnemyMovingOutCharacter::ResetRotationCooldown, 0.2f, false);
	
	}

// 다시 처음으로 돌아가게
void AEnemyMovingOutCharacter::ResetRotationCooldown()
{
	Bhashit = false;
}
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
	// 틱마다 fVector x 축으로 쭉 간다
	FVector Direction = GetActorForwardVector();
	AddMovementInput(Direction, 1.0f);
	
	
}
