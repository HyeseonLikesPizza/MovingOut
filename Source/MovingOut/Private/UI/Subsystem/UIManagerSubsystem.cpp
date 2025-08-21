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
#include "UI/Widget/InGameOverlayWidget.h"
#include "UI/Widget/TitleScreenWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"


UUIManagerSubsystem::UUIManagerSubsystem()
{
	// UISettings 등록
	
	static ConstructorHelpers::FObjectFinder<UUISettingAsset> UISettingBP(TEXT("/Game/Blueprints/Data/DA_UISettingAsset.DA_UISettingAsset"));
	if (ensureMsgf(UISettingBP.Succeeded(), TEXT("DA_UISettingAsset not found. Check path.")))
	{
		UISettings = UISettingBP.Object;
	}

	// Controller Class Map 추가
	
	ControllerClassMap.Add(EUIScreen::InGame, UOverlayWidgetController::StaticClass());
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

void UUIManagerSubsystem::ShowScreen(EUIScreen Screen)
{
	UUserWidget* Target = CreateIfNeeded(Screen);
	if (!Target) return;
	

	// 기존 메인 화면 내리기
	if (Current.IsValid())
		Current->RemoveFromParent();


	
	// 화면별 컨트롤러 주입 (필요한 화면들만)
	switch (Screen)
	{
	case EUIScreen::Title:
		{
			// Title Widget은 위젯 컨트롤러 필요 없음
		}
	case EUIScreen::MainMenu:
		{
			// Main Menu 는 위젯 컨트롤러 필요 없음	
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
			// if (auto* ResultUI = Cast<UResultWidget>(Target))
			// {
			// 	auto* WC = GetOrCreateController<UResultWidgetController>(EUIScreen::Result);
			// 	ResultUI->SetWidgetController(WC);
			// }
			// break;
		}
	case EUIScreen::Pause:
	default:
		break;
	}


	// 뷰포트에 위젯 추가
	Target->AddToViewport(0);
	Current = TWeakObjectPtr<UUserWidget>(Target);
	
	
	
	// 입력 모드 맞추기
	switch (Screen)
	{
		case EUIScreen::Title:
			SetInputModeUIOnly();
			break;
		case EUIScreen::MainMenu:
			SetInputModeUIOnly();
			break;
		case EUIScreen::InGame:
			SetInputModeGameOnly();
			break;
		case EUIScreen::Pause:
			SetInputModeUIOnly(Target);
			break;
		case EUIScreen::Result:
			SetInputModeUIOnly(Target);
			break;
	}
	
}

UUserWidget* UUIManagerSubsystem::GetScreenWidget(EUIScreen Screen) const
{
	if (const TWeakObjectPtr<UUserWidget>* P = ScreenCache.Find(Screen))
		return P->Get();
	return nullptr;
}

void UUIManagerSubsystem::ShowModal(TSubclassOf<UUserWidget> ModalClass, int32 ZOrder)
{
	if (!ModalClass) return;
	APlayerController* PC = GetPC();
	if (!PC) return;
	UUserWidget* Modal = CreateWidget<UUserWidget>(PC, ModalClass);
	if (!Modal) return;

	Modal->AddToViewport(ZOrder);
	ModalStack.Push(Modal);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, Modal);
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

void UUIManagerSubsystem::BindGameStateSignals()
{
	if (UWorld* World = GetWorld())
	{
		if (auto* GS = World->GetGameState<AMovingOutGameState>())
		{
			// 예시: bPlayStopped가 true가 되면 결과 화면으로
			// GameState에 OnMatchFinished 같은 델리게이트가 있다면 그걸 듣는 게 가장 깔끔
			// 여기서는 폴링 대신 간단 루프타이머나 OnRep로 연결하는 방식을 추천

			// 예) 루프 타이머로 상태 감시(가벼운 주기)
			FTimerHandle H;
			World->GetTimerManager().SetTimer(H, [this, GS]()
			{
				static bool bPrevStopped = false;
				if (GS->bPlayStopped && !bPrevStopped)
				{
					bPrevStopped = true;
					ShowScreen(EUIScreen::Result);
				}
			}, 0.1f, true);
		}
	}
}

void UUIManagerSubsystem::SetInputModeGameOnly()
{
	if (APlayerController* PC = GetPC())
	{
		FInputModeGameOnly M;
		PC->SetInputMode(M);
		PC->bShowMouseCursor = false;
	}
}

void UUIManagerSubsystem::SetInputModeUIOnly(UUserWidget* Focus)
{
	if (APlayerController* PC = GetPC())
	{
		FInputModeUIOnly M;
		if (Focus) M.SetWidgetToFocus(Focus->TakeWidget());
		M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(M);
		PC->bShowMouseCursor = true;

		//Focus->SetKeyboardFocus();
		//Focus->SetUserFocus(PC);
	}
}

void UUIManagerSubsystem::SetInputModeGameAndUI(UUserWidget* Focus)
{
	if (APlayerController* PC = GetPC())
	{
		
		FInputModeGameAndUI M;
		if (Focus) M.SetWidgetToFocus(Focus->TakeWidget());
		M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
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
	if (Cls==nullptr) return nullptr;

	APlayerController* PC = GetPC();
	if (!PC) return nullptr;

	UUserWidget* W = CreateWidget<UUserWidget>(PC, Cls);
	ScreenCache.Add(Screen, W);
	
	return W;
}

TSubclassOf<UUserWidget> UUIManagerSubsystem::ResolveClass(EUIScreen Screen) const
{
	switch (Screen)
	{
	case EUIScreen::Title:
		return TitleScreenClass;
	case EUIScreen::InGame:
		return OverlayHUDClass;
	case EUIScreen::Pause:
		return PauseMenuClass;
	case EUIScreen::Result:
		return ResultScreenClass;
	}
	return nullptr;
}
