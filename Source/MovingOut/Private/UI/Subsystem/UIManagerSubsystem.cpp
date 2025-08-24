// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subsystem/UIManagerSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Overlay.h"
#include "Data/UISettingAsset.h"
#include "Game/MovingOutGameState.h"
#include "Game/MovingOutGameMode.h"
#include "UI/WidgetController/BaseWidgetController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widget/InGameOverlayWidget.h"
#include "UI/Widget/IntroWidget.h"
#include "UI/Widget/MainMenuScreenWidget.h"
#include "UI/Widget/PauseWidget.h"
#include "UI/Widget/TitleScreenWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widget/PauseWidget.h"
#include "UI/Widget/SelectStageWidget.h"
#include "UI/Widget/StageInfoWidget.h"


UUIManagerSubsystem::UUIManagerSubsystem()
{
	// UISettings 등록

	static ConstructorHelpers::FObjectFinder<UUISettingAsset> UISettingBP(
		TEXT("/Game/Blueprints/Data/DA_UISettingAsset.DA_UISettingAsset"));
	if (ensureMsgf(UISettingBP.Succeeded(), TEXT("DA_UISettingAsset not found. Check path.")))
	{
		UISettings = UISettingBP.Object;
	}

	// Controller Class Map 추가

	ControllerClassMap.Add(EUIScreen::InGame, UOverlayWidgetController::StaticClass());

	// InitialScreen 초기화
	InitialScreen = EUIScreen::Title;
}

UUserWidget* UUIManagerSubsystem::GetCurrentWidget() const
{
	return Current.Get();
}

UBaseWidgetController* UUIManagerSubsystem::GetController(EUIScreen Screen)
{
	if (ControllerCache.Contains(Screen))
	{
		TObjectPtr<UBaseWidgetController> Found = ControllerCache[Screen];
		return Found.Get();
	}

	if (!ControllerClassMap.Contains(Screen))
	{
		return nullptr;
	}
	else
	{
		TSubclassOf<UBaseWidgetController> Cls = ControllerClassMap[Screen];
		UBaseWidgetController* WC = NewObject<UBaseWidgetController>(this, Cls);

		if (auto* PC = GetPC())
		{
			auto* GS = GetWorld()->GetGameState<AMovingOutGameState>();
			auto* GM = GetWorld()->GetAuthGameMode<AMovingOutGameMode>();
			WC->InitializeController(PC, GS, GM);
			WC->Bind();
		}

		ControllerCache.Add(Screen, TObjectPtr<UBaseWidgetController>(WC));

		return WC;
	}
}

void UUIManagerSubsystem::RegisterControllerClass(EUIScreen Screen, TSubclassOf<UBaseWidgetController> Class)
{
	ControllerClassMap.Add(Screen, Class);
}

void UUIManagerSubsystem::ApplyInputModeForScreen(EUIScreen Screen, UUserWidget* Target)
{
	switch (Screen)
	{
	case EUIScreen::Title:
		SetInputModeUIOnly(Target);
		break;
	case EUIScreen::MainMenu:
		SetInputModeUIOnly(Target);
		break;
	case EUIScreen::Intro:
		SetInputModeUIOnly(Target);
		break;
	case EUIScreen::StageInfo:
		SetInputModeUIOnly(Target);
		break;
	case EUIScreen::InGame:
		SetInputModeGameAndUI();
		break;
	case EUIScreen::Pause:
		SetInputModeUIOnly(Target);
		break;
	case EUIScreen::Result:
		SetInputModeUIOnly(Target);
		break;
	}
}

void UUIManagerSubsystem::WireTitleScreen(UTitleScreenWidget* Widget)
{
	Widget->OnStartRequested.RemoveAll(this);
	Widget->OnStartRequested.AddDynamic(this, &UUIManagerSubsystem::HandleStartRequested);
}

void UUIManagerSubsystem::WireMainMenu(UMainMenuScreenWidget* Widget)
{
	Widget->OnRequestIntro.RemoveAll(this);
	Widget->OnRequestIntro.AddDynamic(this, &UUIManagerSubsystem::HandleRequestIntro);
}

void UUIManagerSubsystem::WirePauseMenu(UPauseWidget* Widget)
{
	Widget->OnRequestResumeGame.RemoveAll(this);
	Widget->OnRequestResumeGame.AddDynamic(this, &UUIManagerSubsystem::HandleResumeGame);

	Widget->OnRequestNewGame.RemoveAll(this);
	Widget->OnRequestNewGame.AddDynamic(this, &UUIManagerSubsystem::HandleRequestNewGame);
}

void UUIManagerSubsystem::WireIntro(UIntroWidget* Widget)
{
	Widget->OnRequestSelectLevel.RemoveAll(this);
	Widget->OnRequestSelectLevel.AddDynamic(this, &UUIManagerSubsystem::HandleRequestSelectStage);
}

void UUIManagerSubsystem::WireSelectStage(USelectStageWidget* Widget)
{
	Widget->OnRequestStageInfo.RemoveAll(this);
	Widget->OnRequestStageInfo.AddDynamic(this, &UUIManagerSubsystem::HandleRequestStageInfo);
}

void UUIManagerSubsystem::WireStageInfo(UStageInfoWidget* Widget)
{
	Widget->OnRequestGameStart.RemoveAll(this);
	Widget->OnRequestGameStart.AddDynamic(this, &UUIManagerSubsystem::HandleRequestNewGame);
}

void UUIManagerSubsystem::HandleStartRequested()
{
	ShowScreen(EUIScreen::MainMenu);
}

void UUIManagerSubsystem::HandleRequestNewGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Requested New Game"));
	if (auto* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
	{
		InitialScreen = EUIScreen::InGame;
		ControllerCache.Empty();
		UGameplayStatics::OpenLevel(PC, FName(TEXT("Stage1")));
	}
}

void UUIManagerSubsystem::HandleRequestIntro()
{
	if (auto* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
	{
		InitialScreen = EUIScreen::Intro;
		ControllerCache.Empty();
		UGameplayStatics::OpenLevel(PC, FName(TEXT("IntroLevel")));
	}
}

void UUIManagerSubsystem::HandleRequestSelectStage()
{
	if (auto* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
	{
		InitialScreen = EUIScreen::SelectStage;
		ControllerCache.Empty();
		UGameplayStatics::OpenLevel(PC, FName(TEXT("SelectStageMap")));
	}
}

void UUIManagerSubsystem::HandleRequestStageInfo()
{
	ShowModal(EUIScreen::StageInfo);
}

void UUIManagerSubsystem::HandleMatchStopped()
{
	if (auto* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
	{
		InitialScreen = EUIScreen::Result;
		ControllerCache.Empty();
		UGameplayStatics::OpenLevel(PC, FName(TEXT("ResultMap")));
	}
}

void UUIManagerSubsystem::HandleResumeGame()
{
	ResumeFromPause();
}

void UUIManagerSubsystem::BindScreenEvents(EUIScreen Screen, UUserWidget* Target)
{
	switch (Screen)
	{
	case EUIScreen::Title:
		{
			if (UTitleScreenWidget* TitleWidget = Cast<UTitleScreenWidget>(Target))
			{
				WireTitleScreen(TitleWidget);
			}
			break;
		}
	case EUIScreen::MainMenu:
		{
			if (UMainMenuScreenWidget* MainMenuWidget = Cast<UMainMenuScreenWidget>(Target))
			{
				WireMainMenu(MainMenuWidget);
			}
			break;
		}
	case EUIScreen::Intro:
		{
			if (UIntroWidget* IntroWidget = Cast<UIntroWidget>(Target))
			{
				WireIntro(IntroWidget);
			}
			break;
		}
	case EUIScreen::SelectStage:
		{
			if (USelectStageWidget* SelectStageWidget = Cast<USelectStageWidget>(Target))
			{
				WireSelectStage(SelectStageWidget);
			}
			break;
		}
	case EUIScreen::StageInfo:
        	{
        		if (UStageInfoWidget* StageInfoWidget = Cast<UStageInfoWidget>(Target))
        		{
        			WireStageInfo(StageInfoWidget);
        		}
        		break;
        	}
	case EUIScreen::InGame:
		{
			BindGameStateSignals();
			break;
		}
	case EUIScreen::Pause:
		{
			if (UPauseWidget* PauseWidget = Cast<UPauseWidget>(Target))
			{
				WirePauseMenu(PauseWidget);
			}
			break;
		}
	}
}

void UUIManagerSubsystem::SetupScreenController(EUIScreen Screen, UUserWidget* Target)
{
	switch (Screen)
	{
	case EUIScreen::Title:
		{
			// Title Widget은 위젯 컨트롤러 필요 없음
			break;
		}
	case EUIScreen::MainMenu:
		{
			// Main Menu 는 위젯 컨트롤러 필요 없음
			break;
		}
	case EUIScreen::Intro:
		{
			// Intro 는 위젯 컨트롤러 필요 없음
			break;
		}
	case EUIScreen::SelectStage:
		{
			// SelectStage 는 위젯 컨트롤러 필요 없음
			break;
		}
	case EUIScreen::InGame:
		{
			if (auto* InGameUI = Cast<UInGameOverlayWidget>(Target))
			{
				UOverlayWidgetController* WC = Cast<UOverlayWidgetController>(GetController(EUIScreen::InGame));
				InGameUI->SetWidgetController(WC);
				WC->Bind();
			}
			break;
		}
	case EUIScreen::Result:
		{
			break;
		}
	case EUIScreen::Pause:
		{
			// Pause Widget은 위젯 컨트롤러 없음
			break;	
		}
	default:
		break;
	}
}

void UUIManagerSubsystem::ShowScreen(EUIScreen Screen)
{
	// Screen 위젯 생성
	UUserWidget* Target = CreateIfNeeded(Screen);
	if (!Target) return;
	CurrentScreen = Screen;

	// 기존 메인 화면 내리기
	if (Current.IsValid())
		Current->RemoveFromParent();

	// 뷰포트에 위젯 추가
	Target->AddToViewport(0);
	Current = TWeakObjectPtr<UUserWidget>(Target);

	// 화면별 컨트롤러 주입 (필요한 화면들만)
 	SetupScreenController(Screen, Target);

	// 델리게이트 구독
	BindScreenEvents(Screen, Target);
	
	// 입력 모드 맞추기
	ApplyInputModeForScreen(Screen, Target);
}

UUserWidget* UUIManagerSubsystem::GetScreenWidget(EUIScreen Screen) const
{
	if (const TWeakObjectPtr<UUserWidget>* P = ScreenCache.Find(Screen))
		return P->Get();
	return nullptr;
}

void UUIManagerSubsystem::ShowModal(EUIScreen Screen, int32 ZOrder)
{
	APlayerController* PC = GetPC();
	if (!PC) return;

	// 모달 위젯 생성
	UUserWidget* Modal = CreateIfNeeded(Screen);
	if (!Modal) return;

	// 델리게이트 바인드
	BindScreenEvents(Screen, Modal);

	// 뷰포트에 추가
	Modal->AddToViewport(ZOrder);

	// 모달 스택에 추가
	ModalStack.Push(Modal);

	// 입력모드 설정
	ApplyInputModeForScreen(Screen, Modal);
}

void UUIManagerSubsystem::CloseTopModal()
{
	while (!ModalStack.IsEmpty())
	{
		TWeakObjectPtr<UUserWidget> Top = ModalStack.Top();
		ModalStack.Pop();
		if (Top.IsValid())
		{
			Top->RemoveFromParent();
			break;
		}
	}


	// 모달이 없어졌다면 메인 화면 포커스로 복귀
	if (ModalStack.IsEmpty() && Current.IsValid())
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetPC(), Current.Get());
}

void UUIManagerSubsystem::TogglePause()
{
	if (!GetWorld()) return;

	if (!bPauseOpen)
	{
		OpenPauseModal_Internal();
	}
	else
	{
		ClosePauseModal_Internal();
	}
}

void UUIManagerSubsystem::ResumeFromPause()
{
	if (bPauseOpen)
	{
		ClosePauseModal_Internal();
	}
}

void UUIManagerSubsystem::HandleEscPressed()
{
	TogglePause();
}


void UUIManagerSubsystem::ApplyInitialUI()
{
	if (AMovingOutGameMode* GM = Cast<AMovingOutGameMode>(UGameplayStatics::GetGameMode(GetPC())))
	{
		ShowScreen(InitialScreen);
	}
}

void UUIManagerSubsystem::BindGameStateSignals()
{
	if (UWorld* World = GetWorld())
	{
		if (auto* GS = World->GetGameState<AMovingOutGameState>())
		{
			GS->OnMatchStopped.AddUniqueDynamic(this, &UUIManagerSubsystem::HandleMatchStopped);
		}
	}
}

void UUIManagerSubsystem::SetInputModeGameOnly()
{
	if (APlayerController* PC = GetPC())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void UUIManagerSubsystem::SetInputModeUIOnly(UUserWidget* Focus)
{
	if (APlayerController* PC = GetPC())
	{
		FInputModeUIOnly M;
		M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(M);
		PC->bShowMouseCursor = true;
		if (Focus)
		{
			M.SetWidgetToFocus(Focus->TakeWidget());
			Focus->SetKeyboardFocus();
			Focus->SetUserFocus(PC);
		}
	}
}

void UUIManagerSubsystem::SetInputModeGameAndUI(UUserWidget* Focus)
{
	if (APlayerController* PC = GetPC())
	{
		FInputModeGameAndUI M;
		M.SetHideCursorDuringCapture(false);
		M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		if (Focus) M.SetWidgetToFocus(Focus->TakeWidget());
		PC->SetInputMode(M);
		PC->bShowMouseCursor = true;
	}
}

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!OverlayHUDClass && UISettings && !UISettings->OverlayHUDClass.IsNull())
	{
		OverlayHUDClass = UISettings->OverlayHUDClass.LoadSynchronous();
	}

	if (!TitleScreenClass && UISettings && !UISettings->TitleScreenWidgetClass.IsNull())
	{
		TitleScreenClass = UISettings->TitleScreenWidgetClass.LoadSynchronous();
	}

	if (!MainMenuScreenClass && UISettings && !UISettings->MainMenuWidgetClass.IsNull())
	{
		MainMenuScreenClass = UISettings->MainMenuWidgetClass.LoadSynchronous();
	}

	if (!IntroWidgetClass && UISettings && !UISettings->IntroWidgetClass.IsNull())
	{
		IntroWidgetClass = UISettings->IntroWidgetClass.LoadSynchronous();
	}

	if (!SelectStageWidgetClass && UISettings && !UISettings->IntroWidgetClass.IsNull())
	{
		SelectStageWidgetClass = UISettings->SelectStageWidgetClass.LoadSynchronous();
	}

	if (!StageInfoWidgetClass && UISettings && !UISettings->StageInfoWidgetClass.IsNull())
	{
		StageInfoWidgetClass = UISettings->StageInfoWidgetClass.LoadSynchronous();
	}

	if (!PauseMenuClass && UISettings && !UISettings->PauseMenuWidgetClass.IsNull())
	{
		PauseMenuClass = UISettings->PauseMenuWidgetClass.LoadSynchronous();
	}

	if (!ResultScreenClass && UISettings && !UISettings->ResultWidgetClass.IsNull())
	{
		ResultScreenClass = UISettings->ResultWidgetClass.LoadSynchronous();
	}
}

void UUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UWorld* UUIManagerSubsystem::GetWorldChecked() const
{
	UWorld* World = GetWorld();
	check(World);
	return World;
}

APlayerController* UUIManagerSubsystem::GetPC() const
{
	return GetLocalPlayer() ? GetLocalPlayer()->GetPlayerController(GetWorld()) : nullptr;
}

UUserWidget* UUIManagerSubsystem::CreateIfNeeded(EUIScreen Screen)
{
	if (ScreenCache.Contains(Screen) && ScreenCache[Screen].IsValid())
		return ScreenCache[Screen].Get();

	TSubclassOf<UUserWidget> Cls = ResolveClass(Screen);
	if (Cls == nullptr) return nullptr;

	APlayerController* PC = GetPC();
	if (!PC) return nullptr;

	UUserWidget* W = CreateWidget<UUserWidget>(PC, Cls);
	if (W)
		ScreenCache.Add(Screen, W);

	return W;
}

TSubclassOf<UUserWidget> UUIManagerSubsystem::ResolveClass(EUIScreen Screen) const
{
	switch (Screen)
	{
	case EUIScreen::Title:
		return TitleScreenClass;
	case EUIScreen::MainMenu:
		return MainMenuScreenClass;
	case EUIScreen::Intro:
		return IntroWidgetClass;
	case EUIScreen::SelectStage:
		return SelectStageWidgetClass;
	case EUIScreen::StageInfo:
		return StageInfoWidgetClass;
	case EUIScreen::InGame:
		return OverlayHUDClass;
	case EUIScreen::Pause:
		return PauseMenuClass;
	case EUIScreen::Result:
		return ResultScreenClass;
	}
	return nullptr;
}

void UUIManagerSubsystem::OpenPauseModal_Internal()
{
	if (!PauseMenuClass) return;
	if (bPauseOpen) return;
	
	// 인게임 상태에서만 열기 
	if (!Current->IsA<UInGameOverlayWidget>()) return;
	
	
	// 실제 게임 일시정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// 모달 위젯 생성 + 스택에 Push (기존 유틸 재사용)
	ScreenCache.Add(EUIScreen::InGame, Current);
	ShowModal(EUIScreen::Pause, /*ZOrder*/ 1000);

	// 캐시(중복생성 방지)
	if (!ModalStack.IsEmpty())
	{
		PauseWidgetWeak = ModalStack.Top(); // 최상단이 방금 띄운 Pause
	}

	bPauseOpen = true;
}

void UUIManagerSubsystem::ClosePauseModal_Internal()
{
	// 스택 최상단이 Pause면 닫기
	if (bPauseOpen)
	{
		// 혹시 Pause가 아닌 게 올라와 있을 수도 있으니 안전하게 탐색
		while (!ModalStack.IsEmpty())
		{
			TWeakObjectPtr<UUserWidget> Top = ModalStack.Top();
			if (!Top.IsValid())
			{
				ModalStack.Pop();
				continue;
			}

			// Pause로 추정되는 위젯을 발견하면 닫고 탈출
			if (PauseWidgetWeak.IsValid() && Top.Get() == PauseWidgetWeak.Get())
			{
				Top->RemoveFromParent();
				ModalStack.Pop();
				break;
			}
			else
			{
				// 다른 모달이 위에 있다면 그것부터 닫음
				Top->RemoveFromParent();
				ModalStack.Pop();
			}
		}

		PauseWidgetWeak = nullptr;
		bPauseOpen = false;

		// 게임 재개
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		// 입력 모드 복귀: 인게임이면 GameOnly로
		if (ScreenCache.Contains(EUIScreen::InGame))
		{
			// 인게임 위젯인지 확인
			UUserWidget* InGameW = GetScreenWidget(EUIScreen::InGame);
			if (InGameW && Current.Get() == InGameW)
			{
				ApplyInputModeForScreen(EUIScreen::InGame, InGameW);
			}
			else
			{
				// 안전장치 (혹시 다른 화면이면 기존 로직대로)
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetPC(), Current.Get());
			}
		}
	}
}
