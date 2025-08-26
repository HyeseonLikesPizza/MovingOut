
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Type/MedalTypes.h"
#include "AdditionalGoalNameData.generated.h"

UCLASS()
class MOVINGOUT_API UAdditionalGoalNameData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FAdditionalGoalData> AdditionalGoals;
	
};
