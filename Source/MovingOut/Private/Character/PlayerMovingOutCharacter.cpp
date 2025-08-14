
#include "Character/PlayerMovingOutCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MovingOut/DebugHelpers.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "MovingOut/MovingOut.h"
#include "Component/InteractiveComponent.h"

APlayerMovingOutCharacter::APlayerMovingOutCharacter()
{
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
	GrabTraceDistance = 200.f;
	GrabDistance = 50.f;

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);

	InteractiveComponent = CreateDefaultSubobject<UInteractiveComponent>(TEXT("InteractiveComponent"));
	LightCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightCone"));
	LightCone->SetupAttachment(RootComponent);
	LightCone->SetVisibility(false);

}

void APlayerMovingOutCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void APlayerMovingOutCharacter::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (!InteractiveComponent->GetIsAming())
	{
		AddMovementInput(ForwardDirection, MovementVector.X);
		AddMovementInput(RightDirection, MovementVector.Y);
	}
	else
	{
		// 1. 입력 방향 벡터의 크기가 0이 아닐 때만 (즉, 키를 누르고 있을 때만) 회전 처리
		if (!MovementVector.IsNearlyZero())
		{
			// 2. 입력 방향 벡터를 정규화(Normalize)하고, 이를 목표 회전값(FRotator)으로 변환합니다.
			// Y(Right)가 X(Forward)보다 우선순위가 높게 처리되지만, 탑다운 시점에서는 큰 차이가 없습니다.
			FVector TargetDirection = FVector(MovementVector.X, MovementVector.Y, 0.f);
			TargetDirection.Normalize();
			FRotator TargetRotation = TargetDirection.Rotation();

			// 3. 현재 캐릭터의 회전값을 가져옵니다.
			FRotator CurrentRotation = GetActorRotation();

			// 4. 현재 회전값에서 목표 회전값으로 부드럽게 보간(RInterpTo)합니다.
			// DeltaTime은 프레임 간 시간 간격이며, RotationSpeed는 회전 속도를 조절합니다.
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->DeltaTimeSeconds, 1.f);

			// 5. 계산된 새로운 회전값을 액터에게 적용합니다.
			GetController()->SetControlRotation(NewRotation); // 컨트롤러의 회전을 변경
			SetActorRotation(NewRotation); // 액터(캐릭터)의 회전을 변경
			
		}
	}
	
}
