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
#include "UI/WidgetController/OverlayWidgetController.h"


UUIManagerSubsystem::UUIManagerSubsystem()
{
	static ConstructorHelpers::FClassFinder<UInGameOverlayWidget> tempWidget(TEXT("/Game/Blueprints/UI/WBP_OverlayWidget.WBP_OverlayWidget_C"));
	if (tempWidget.Succeeded())
	{
		OverlayHUDClass = tempWidget.Class;
	}
}

UBaseWidgetController* UUIManagerSubsystem::GetController(EUIScreen Screen)
{
	if (UBaseWidgetController* Found = (ControllerCache.Find(Screen)->Get()))
	{
		return Found;
	}

	TSubclassOf<UBaseWidgetController> Cls = ControllerClassMap.Find(Screen)->Get();
	if (!Cls) return nullptr;

	UBaseWidgetController* WC = NewObject<UBaseWidgetController>(GetWorld(), Cls);

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

	// 2) 화면별 컨트롤러 주입 (필요한 화면들만)
	switch (Screen)
	{
	case EUIScreen::InGame:
		{
			if (auto* InGameUI = Cast<UInGameOverlayWidget>(Target))
			{
				UOverlayWidgetController* WC = Cast<UOverlayWidgetController>(GetController(EUIScreen::InGame));
				InGameUI->SetWidgetController(WC);
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

	Target->AddToViewport(0);
	Current = TWeakObjectPtr<UUserWidget>(Target);
	

	APlayerController* PC = GetPC();

	
	// 입력 모드 맞추기
	switch (Screen)
	{
		case EUIScreen::InGame:
			SetInputModeGameOnly();
			break;
		case EUIScreen::Pause:
			SetInputModeUIOnly();
			break;
		case EUIScreen::Result:
			SetInputModeUIOnly();
			break;
	}
	
}

UUserWidget* UUIManagerSubsystem::GetScreenWidget(EUIScreen Screen) const
{
	if (UUserWidget* Found = ScreenCache.Find(Screen)->Get())
		return Found;
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
		TWeakObjectPtr<UUserWidget> Top = ModalStack.Top().Get();
		ModalStack.Pop();
		if (Top.Get())
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
	
	//RegisterControllerClass(EUIScreen::InGame, UOverlayWidgetController::StaticClass());
	//OverlayHUDClass = UInGameOverlayWidget::StaticClass();
	
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OverlayHUDClass->GetName());

	//if (UISettings && UISettings->OverlayHUDClass.IsValid())
	//	OverlayHUDClass = UISettings->OverlayHUDClass.LoadSynchronous();
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

	// 컨트롤러 주입이 필요하면 여기서 호출
	// if (InGameWidgetController && Screen == EUIScreen::InGame)
	//     W->CallFunctionByNameWithArguments(TEXT("SetWidgetController"), ...); // 또는 C++ 캐스팅 후 세터 호출

	return W;
}

TSubclassOf<UUserWidget> UUIManagerSubsystem::ResolveClass(EUIScreen Screen) const
{
	switch (Screen)
	{
	case EUIScreen::InGame: return OverlayHUDClass;
	case EUIScreen::Pause:  return PauseMenuClass;
	case EUIScreen::Result: return ResultScreenClass;
	}
	return nullptr;
}
