#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


struct FMovingOutGameplayTags
{
public:
	static const FMovingOutGameplayTags& Get() { return GameplayTags;  }
	static void InitializeNativeGameplayTags();


	/* Breakable Actors */
	FGameplayTag Breakable_Window;
	

private:
	static FMovingOutGameplayTags GameplayTags;
};