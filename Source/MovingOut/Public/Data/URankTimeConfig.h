
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MovingOut/Public/Type/MedalTypes.h"
#include "URankTimeConfig.generated.h"

UCLASS()
class MOVINGOUT_API UURankTimeConfig : public UDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Medal")
	FMedalThresholds Thresholds;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override
	{
		Super::PostEditChangeProperty(Event);
		Thresholds.Normalize();
	}
#endif
	
};
