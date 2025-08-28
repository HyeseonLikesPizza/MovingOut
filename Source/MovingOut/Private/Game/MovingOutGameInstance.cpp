
#include "Game/MovingOutGameInstance.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"

void UMovingOutGameInstance::Init()
{
	Super::Init();

	// 맵 로드 콜백 바인딩
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UMovingOutGameInstance::BeginLoadingScreen);
	//FCoreUObjectDelegates::PostLoadMapWithWorld.Add(this, &UMovingOutGameInstance::EndLoadingScreen);
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UMovingOutGameInstance::BeginLoadingScreen_WithContext);
}

void UMovingOutGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (IsRunningDedicatedServer()) return;

	/*
	// UMG 로딩 위젯 생성
	UWorld* World = GetWorld();
	if (!World) return;

	UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Blueprints/UI/WBP_Loading.WBP_Loading_C"));
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading Widget is not loaded"));
		return;
	}

	UUserWidget* W = CreateWidget<UUserWidget>(World, WidgetClass);
	LoadingUMG = W;

	*/
	
	// MoviePlayer 속성
	FLoadingScreenAttributes Attr;
	Attr.bAutoCompleteWhenLoadingCompletes = true; // 로딩 끝나면 자동 종료
	Attr.MinimumLoadingScreenDisplayTime = 2.f; // 너무 빨리 깜빡임 방지
	Attr.bWaitForManualStop = false; // 수동 종료 여부

	Attr.MoviePaths = { TEXT("Loading")};
	
	
	
	// UMG -> Slate 위젯으로 변환해 연결
	//Attr.WidgetLoadingScreen = W->TakeWidget();

	GetMoviePlayer()->SetupLoadingScreen(Attr);
	GetMoviePlayer()->PlayMovie();
	
}

void UMovingOutGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
}

void UMovingOutGameInstance::BeginLoadingScreen_WithContext(const FWorldContext& inWorldContext, const FString& MapName)
{
	BeginLoadingScreen(MapName);
}
