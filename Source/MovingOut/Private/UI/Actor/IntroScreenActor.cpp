
#include "UI/Actor/IntroScreenActor.h"

/*

#include "Runtime/MediaAssets/Public/MediaSoundComponent.h"

AIntroScreenActor::AIntroScreenActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	IntroMediaPlayer = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("MediaPlayer"));
	IntroMediaPlayer->SetupAttachment(GetRootComponent());

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(IntroMediaPlayer);
	AudioComponent->bAutoActivate = false;
}

void AIntroScreenActor::BeginPlay()
{
	Super::BeginPlay();

	if (IntroMediaPlayer)
	{
		// IntroMediaPlayer End 에 바인딩 하기
	}
}

void AIntroScreenActor::HandleIntroEnded()
{
	// 브로드캐스트
}
*/