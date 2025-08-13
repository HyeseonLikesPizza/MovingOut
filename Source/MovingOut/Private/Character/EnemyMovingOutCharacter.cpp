
#include "Character/EnemyMovingOutCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/PlayerMovingOutCharacter.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationPath.h"
#include "AIController.h"

// 생성자
AEnemyMovingOutCharacter::AEnemyMovingOutCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	
	// 네비게이션 시스템을 사용하려면 반드시 필요
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 플레이어 감지용 SphereComponent 생성 및 설정
	PlayerDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectionSphere"));
	PlayerDetectionSphere->SetupAttachment(RootComponent);
	PlayerDetectionSphere->SetSphereRadius(500.f); // 감지 범위
	PlayerDetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	// SphereComponent의 Overlap 이벤트에 함수를 바인딩
	PlayerDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerDetected);
	PlayerDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyMovingOutCharacter::OnPlayerLost);

	CurrentState = EEnemyState::ES_Idle;
}

// 게임 시작 시 호출
void AEnemyMovingOutCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 캡슐 컴포넌트가 있는지 확인 후 충돌 이벤트 바인딩
	if(GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyMovingOutCharacter::OnEnemyHit);

		
	}
   
	
	// 게임 시작하고 n초뒤 패트롤
	FTimerHandle StartPatrolTimer;
	GetWorldTimerManager().SetTimer(StartPatrolTimer, this, &AEnemyMovingOutCharacter::FindAndMoveToNewPatrolDestination, 1.0f, false);
	UE_LOG(LogTemp, Warning, TEXT("Patrol Timer Started"));
	SetEnemyState(EEnemyState::ES_Patrolling);
	
}
void AEnemyMovingOutCharacter::StartPatrolling()
{
	SetEnemyState(EEnemyState::ES_Patrolling);
	FindAndMoveToNewPatrolDestination();
	
}


void AEnemyMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 현재 상태에 따라 다른 로직을 실행
	switch (CurrentState)
	{
		case EEnemyState::ES_Idle:
			HandleIdle();
			break;
		case EEnemyState::ES_Patrolling:
			HandlePatrolling(DeltaSeconds);
			break;
		case EEnemyState::ES_Chasing:
			HandleChasing(DeltaSeconds);
			break;
		case EEnemyState::ES_HitReaction:
			HandleHitReaction(DeltaSeconds);
			break;
	}
}

// AI 상태 변경 함수
void AEnemyMovingOutCharacter::SetEnemyState(EEnemyState NewState)
{
	if (CurrentState == NewState) return;

	// HitReaction 상태로 들어가기 전에 현재 상태를 저장 
	if (NewState == EEnemyState::ES_HitReaction)
	{
		PreviousState = CurrentState;
	}

	CurrentState = NewState;
	UE_LOG(LogTemp, Warning, TEXT("AI State Changed to: %s"), *UEnum::GetValueAsString(NewState));
}


void AEnemyMovingOutCharacter::HandleIdle()
{
	// 대기 상태에서는 아무것도 하지 않음.
}

void AEnemyMovingOutCharacter::HandlePatrolling(float DeltaTime)
{
	// 목표 지점에 거의 도달했는지 확인
	const float DistanceToGoal = FVector::Dist(GetActorLocation(), PatrolDestination);
	if (DistanceToGoal < 150.f)
	{
		// 목표에 도달했으므로, 새로운 순찰 지점을 찾아 이동
		FindAndMoveToNewPatrolDestination();
	}
}

void AEnemyMovingOutCharacter::HandleChasing(float DeltaTime)
{
	if (PlayerTarget && PlayerTarget->IsValidLowLevel())
	{
		// 네비게이션 시스템을 사용해 플레이어를 향해 이동
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), PlayerTarget);
	}
	else
	{
		// 타겟(플레이어) 참조가 유효하지 않으면 순찰 상태로 돌아감.
		PlayerTarget = nullptr;
		SetEnemyState(EEnemyState::ES_Patrolling);
		FindAndMoveToNewPatrolDestination();
	}
}

void AEnemyMovingOutCharacter::HandleHitReaction(float DeltaTime)
{
	// 목표 회전값으로 부드럽게 회전
	FRotator CurrentRotation = GetActorRotation();
	FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
	SetActorRotation(InterpolatedRotation);
}

// 이벤트 함수와 아닌 함수를 따로 정리함( 아래는 모두 이벤트 함수, 추가도 꼭 똑같이 해라 재민아 까먹지 말고 [과거의 재민이가]
void AEnemyMovingOutCharacter::OnPlayerDetected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 감지된 액터가 플레이어인지 확인
	APlayerMovingOutCharacter* Player = Cast<APlayerMovingOutCharacter>(OtherActor);
	if (Player)
	{
		// 현재 벽에 부딪힌 상태가 아닐 때만 추적 상태로 변경
		if (CurrentState != EEnemyState::ES_HitReaction)
		{
			PlayerTarget = Player;
			SetEnemyState(EEnemyState::ES_Chasing);
		}
	}
}

void AEnemyMovingOutCharacter::OnPlayerLost(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 범위를 벗어난 액터가 현재 추적 중인 플레이어인지 확인
	if (OtherActor == PlayerTarget)
	{
		PlayerTarget = nullptr;
		
		// 추적 중인 상태에서만 순찰 상태로 변경
		if(CurrentState == EEnemyState::ES_Chasing)
		{
			SetEnemyState(EEnemyState::ES_Patrolling);
			// 플레이어를 잃어버렸으므로 즉시 새로운 순찰 지점으로 이동 시작
			FindAndMoveToNewPatrolDestination();
		}
	}
}

void AEnemyMovingOutCharacter::OnEnemyHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 자기 자신이나 플레이어와의 충돌은 무시하고, 월드 오브젝트(벽 등)와의 충돌만 처리
	if (OtherActor && OtherActor != this && !Cast<APlayerMovingOutCharacter>(OtherActor))
	{
		// 진행 방향과 충돌 지점의 법선 벡터를 통해 정면 충돌에 가까운지 확인
		float ForwardDot = FVector::DotProduct(GetActorForwardVector(), -Hit.ImpactNormal);

		// 정면으로 부딪혔을 때만 반응하도록 하여, 스치거나 옆으로 부딪히는 일을 무시.
		if (ForwardDot > 0.5f)
		{
			// 현재 이동을 멈춥
			AController* AIController = GetController();
			if (AIController)
			{
				UAIBlueprintHelperLibrary::GetCurrentPath(AIController)->PathPoints.Last();
			}
			
			// 충돌 반응 상태로 전환
			SetEnemyState(EEnemyState::ES_HitReaction);

			// 약간의 랜덤성을 추가하여 뒤로 돌거나 좌/우로 회전
			TargetRotation = GetActorRotation().Add(0.0f, FMath::RandBool() ? 110.f : -110.f, 0.0f);

			// 0.7초 후에 HitReaction 상태를 종료하도록 타이머 설정
			GetWorldTimerManager().SetTimer(HitReactionTimer, this, &AEnemyMovingOutCharacter::EndHitReaction, 0.7f, false);
		}
	}
}

// ai 행동 (nav 시스템 고치면 사용 가능)
void AEnemyMovingOutCharacter::FindAndMoveToNewPatrolDestination()
{
	// 컨트롤러가 유효한지 확인
	AController* AIController = GetController();
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("FindAndMoveToNewPatrolDestination FAILED: AIController is NULL."));
		return;
	}

	//  내비게이션 시스템이 유효한지 확인
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Error, TEXT("FindAndMoveToNewPatrolDestination FAILED: NavigationSystem is NULL."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Attempting to find new patrol destination with Radius: %f"), PatrolRadius);

	FNavLocation RandomLocation;
	//실제로 길을 찾는지 확인
	bool bFound = NavSys->GetRandomReachablePointInRadius(GetActorLocation(), PatrolRadius, RandomLocation);
    
	if (bFound)
	{
		PatrolDestination = RandomLocation.Location;
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS! New Destination: %s"), *PatrolDestination.ToString());
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolDestination);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED to find reachable point in radius. Check NavMesh or PatrolRadius value."));
		// 길 찾기 실패 시, 2초 뒤에 다시 시도
		FTimerHandle RetryTimer;
		GetWorldTimerManager().SetTimer(RetryTimer, this, &AEnemyMovingOutCharacter::FindAndMoveToNewPatrolDestination, 2.0f, false);
	}
}
void AEnemyMovingOutCharacter::EndHitReaction()
{
	GetWorldTimerManager().ClearTimer(HitReactionTimer);
	
	// 이전 상태 로 복귀
	SetEnemyState(PreviousState);

	// 이전 상태가 순찰이었다면 새로운 길 찾기 시작
	if (PreviousState == EEnemyState::ES_Patrolling)
	{
		FindAndMoveToNewPatrolDestination();
	}
	// 이전 상태가 추적이였다면 추적을 자동으로 재개합
}

