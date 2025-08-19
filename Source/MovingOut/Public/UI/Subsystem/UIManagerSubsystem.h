
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIManagerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EUIScreen : uint8
{
	Intro,
	Loading,
	InGame,
	Pause,
	Result
};

UCLASS()
class MOVINGOUT_API UUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	
	// 전환 대상 위젯 클래스들 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> OverlayHUDClass;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PauseMenuClass;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ResultScreenClass;

	// 진입/전환 API
	UFUNCTION(BlueprintCallable) void ShowScreen(EUIScreen Screen);
	UFUNCTION(BlueprintCallable) UUserWidget* GetScreenWidget(EUIScreen Screen) const;
	UFUNCTION(BlueprintCallable) void ShowModal(TSubclassOf<UUserWidget> ModalClass, int32 ZOrder = 1000);
	UFUNCTION(BlueprintCallable) void CloseTopModal();

	// GameState 신호에 반응하여 자동 화면 전환(결과 화면 등)
	UFUNCTION(BlueprintCallable) void BindGameStateSignals();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	// 캐시: 화면별 위젯 인스턴스
	TMap<EUIScreen, TWeakObjectPtr<UUserWidget>> ScreenCache;
	// 현재 표시 중인 메인 화면
	TWeakObjectPtr<UUserWidget> Current;
	// 모달 스택 (위에 올린 순서대로)
	TArray<TWeakObjectPtr<UUserWidget>> ModalStack;

	UWorld* GetWorldChecked() const;
	APlayerController* GetPC() const;

	UUserWidget* CreateIfNeeded(EUIScreen Screen);
	TSubclassOf<UUserWidget> ResolveClass(EUIScreen Screen) const;
};
