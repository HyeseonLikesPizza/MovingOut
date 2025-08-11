#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

// 선 그리기 (기본 빨간색)
FORCEINLINE void DrawDebugLineTrace(
	UWorld* World,
	const FVector& Start,
	const FVector& End,
	FColor Color = FColor::Red,
	float Duration = 2.0f
)
{
	if (World) DrawDebugLine(World, Start, End, Color, false, Duration, 0, 1.5f);
}


// 히트 포인트 시각화
FORCEINLINE void DrawDebugHitPoint(
	UWorld* World,
	const FHitResult& Hit,
	FColor Color = FColor::Green,
	float Radius = 10.f,
	float Duration = 2.f
)
{
	if (World && Hit.bBlockingHit)
	{
		DrawDebugSphere(World, Hit.ImpactPoint, Radius, 12, Color, false, Duration);
	}
}


/*

FORCEINLINE void PrintLog(
	UEngine* Engine,
	const FString& Message,
	uint64 key = 0,
	float TimeToDisplay = 5.f,
	FColor DisplayColor = FColor::Magenta
)
{
	if (Engine)
	{
		FString Msg = FString::Printf(TEXT("Component Name : %s"), *Hit.GetComponent()->GetName());
		Engine->AddOnScreenDebugMessage(key, TimeToDisplay, DisplayColor, Message);
	}
}

*/