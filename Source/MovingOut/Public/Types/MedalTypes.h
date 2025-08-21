#pragma once
#include "CoreMinimal.h"
#include "MedalTypes.generated.h"


UENUM(BlueprintType)
enum class EMedal : uint8
{
	None   UMETA(DisplayName="None"),
	Bronze UMETA(DisplayName="Bronze"),
	Silver UMETA(DisplayName="Silver"),
	Gold   UMETA(DisplayName="Gold"),
	Fail   UMETA(DisplayName="Fail")
};

USTRUCT(BlueprintType)
struct FMedalThresholds
{
	GENERATED_BODY()

	// 단위: 초(Seconds) 권장. (FTimespan 편집/블루프린트 지원이 애매해서 보통 float/int를 씀)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Medal")
	float GoldWithinSeconds  = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Medal")
	float SilverWithinSeconds = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Medal")
	float BronzeWithinSeconds = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Medal")
	float FailOverSeconds = 250.f;

	// 평가 함수(선택): 경과 시간으로 메달 등급 계산
	EMedal Evaluate(float ElapsedSeconds) const
	{
		if (ElapsedSeconds <= GoldWithinSeconds)   return EMedal::Gold;
		if (ElapsedSeconds <= SilverWithinSeconds) return EMedal::Silver;
		if (ElapsedSeconds <= BronzeWithinSeconds) return EMedal::Bronze;
		if (ElapsedSeconds <= FailOverSeconds) return EMedal::Fail;
		return EMedal::None;
	}

	// 유효성 체크(선택): 에디터에서 값 꼬임 방지
	void Normalize()
	{
		// Gold <= Silver <= Bronze 보장
		SilverWithinSeconds = FMath::Max(SilverWithinSeconds, GoldWithinSeconds);
		BronzeWithinSeconds = FMath::Max(BronzeWithinSeconds, SilverWithinSeconds);
	}
	
};
