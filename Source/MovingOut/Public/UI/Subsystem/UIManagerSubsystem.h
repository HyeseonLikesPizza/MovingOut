
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIManagerSubsystem.generated.h"

class UTitleScreenWidget;
class UMainMenuScreenWidget;

UENUM(BlueprintType)
enum class EUIScreen : uint8
{
	None,
	Title,
	Loading,
	MainMenu,
	InGame,
	Pause,
	Result
};

UCLASS()
class MOVINGOUT_API UUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UUIManagerSubsystem();

	// Getter
	UUserWidget* GetCurrentWidget() const;

	
	// 전환 대상 위젯 클래스들 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> TitleScreenClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> MainMenuScreenClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> OverlayHUDClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> PauseMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ResultScreenClass;

	// 읽기 전용 접근자
	UFUNCTION(BlueprintPure, Category="UI")
	UBaseWidgetController* GetController(EUIScreen Screen);

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<class UUISettingAsset> UISettings;

	// 제한적 등록/교체(필요할 때만 열기)
	UFUNCTION(BlueprintCallable, Category="UI")
	void RegisterControllerClass(EUIScreen Screen, TSubclassOf<UBaseWidgetController> Class);
	void ApplyInputModeForScreen(EUIScreen Screen, UUserWidget* Target);

	// 델리게이트 바인딩
	void WireTitleScreen(UTitleScreenWidget* Widget);
	void WireMainMenu(UMainMenuScreenWidget* Widget);

	UFUNCTION()
	void HandleStartRequested(); // Title -> MainMenu

	UFUNCTION()
	void HandleRequestNewGame(); // MainMenu -> Stage1

	// 진입/전환 API
	UFUNCTION(BlueprintCallable)
	void ShowScreen(EUIScreen Screen);
	
	UFUNCTION(BlueprintCallable)
	UUserWidget* GetScreenWidget(EUIScreen Screen) const;
	
	UFUNCTION(BlueprintCallable)
	void ShowModal(TSubclassOf<UUserWidget> ModalClass, int32 ZOrder = 1000);
	
	UFUNCTION(BlueprintCallable)
	void CloseTopModal();


	// 레벨 오픈 시 게임 모드의 Initial Screen에 따라 UI를 띄움
	void ApplyInitialUI();
	

	// GameState 신호에 반응하여 자동 화면 전환(결과 화면 등)
	UFUNCTION(BlueprintCallable)
	void BindGameStateSignals();


	// Widget Input Mode
	UFUNCTION(BlueprintCallable)
	void SetInputModeGameOnly();
	
	UFUNCTION(BlueprintCallable)
	void SetInputModeUIOnly(UUserWidget* Focus = nullptr);
	
	UFUNCTION(BlueprintCallable)
	void SetInputModeGameAndUI(UUserWidget* Focus = nullptr);

	

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


	// 설정(편집 전용) - 화면별로 어떤 컨트롤러 클래스를 쓸지
	UPROPERTY(EditDefaultsOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TMap<EUIScreen, TSubclassOf<UBaseWidgetController>> ControllerClassMap;

private:

	// 캐시(런타임 인스턴스)
	UPROPERTY(Transient)
	TMap<EUIScreen, TObjectPtr<UBaseWidgetController>> ControllerCache;
	
	// 캐시: 화면별 위젯 인스턴스
	UPROPERTY()
	TMap<EUIScreen, TWeakObjectPtr<UUserWidget>> ScreenCache;
	TWeakObjectPtr<UUserWidget> Current;
	
	// 모달 스택 (위에 올린 순서대로)
	UPROPERTY()
	TArray<TWeakObjectPtr<UUserWidget>> ModalStack;

	UWorld* GetWorldChecked() const;
	
	APlayerController* GetPC() const;

	UUserWidget* CreateIfNeeded(EUIScreen Screen);
	TSubclassOf<UUserWidget> ResolveClass(EUIScreen Screen) const;

	EUIScreen InitialScreen;
};
