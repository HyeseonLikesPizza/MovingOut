
#include "MovingOutGameplayTags.h"
#include "GameplayTagsManager.h"

FMovingOutGameplayTags FMovingOutGameplayTags::GameplayTags;

void FMovingOutGameplayTags::InitializeNativeGameplayTags()
{
	/* Breakable Actors */
	GameplayTags.Breakable_Window = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Breakable.Window"), FString("창문"));
}
