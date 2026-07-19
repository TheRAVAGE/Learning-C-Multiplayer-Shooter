// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSubsystem.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#define Print(text) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, text); }

void UMenu::MenuSetup(TSoftObjectPtr<UWorld> Lobby, int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	LobbyLevel = Lobby;
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSubsystem>();
	}
	if (MultiplayerSubsystem)
	{
		MultiplayerSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessionComplete);
		MultiplayerSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSessionComplete);
		MultiplayerSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySessionComplete);
		MultiplayerSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSessionComplete);
		
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (CreateButton)
	{
		CreateButton->OnClicked.AddDynamic(this, &ThisClass::CreateButtonClicked);
	}
	
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::CreateButtonClicked()
{
	Print("Create Button Clicked");
	CreateButton->SetIsEnabled(false);
	if (MultiplayerSubsystem)
	{
		MultiplayerSubsystem->CreateSession(NumPublicConnections, MatchType);
		
	}
}

void UMenu::JoinButtonClicked()
{
	Print("Join Button Clicked");
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSubsystem)
	{
		MultiplayerSubsystem->FindSessions(10000);
	}
}

void UMenu::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			// World->ServerTravel("/Game/Maps/Lobby?listen");
			// UGameplayStatics::OpenLevel(World, FName("Lobby"), true, "listen");
			if (!LobbyLevel.IsNull())
			{
				
				UGameplayStatics::OpenLevelBySoftObjectPtr(World, LobbyLevel, true, "listen");
			}
			else
			{
				Print("Lobby Level isn't valid")
				CreateButton->SetIsEnabled(true);
			}
			
		}
	}
	else
	{
		CreateButton->SetIsEnabled(true);
	}
	Print(FString::Printf(
		TEXT("OnCreateSessionComplete: %s"), 
		bWasSuccessful ? TEXT("Successful") : TEXT("Failed")
		));
}

void UMenu::OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	Print(FString::Printf(
		TEXT("OnFindSessionComplete with %d results: %s"), 
		SessionResults.Num(), 
		bWasSuccessful ? TEXT("Successful") : TEXT("Failed")
		));
	
	if (MultiplayerSubsystem == nullptr) {return;}
	
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSubsystem->JoinSession(Result);
			return;
		}
	}
	
	if (bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	Print(FString::Printf(
		TEXT("OnJoinSessionComplete: %s"), 
		LexToString(Result)
		));
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			Print(FString::Printf(
				TEXT("Connect String: %s"), 
				*Address
				));
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySessionComplete(bool bWasSuccessful)
{
	Print(FString::Printf(
		TEXT("OnDestroySessionComplete: %s"), 
		bWasSuccessful ? TEXT("Successful") : TEXT("Failed")
		));
}

void UMenu::OnStartSessionComplete(bool bWasSuccessful)
{
	Print(FString::Printf(
		TEXT("OnStartSessionComplete: %s"), 
		bWasSuccessful ? TEXT("Successful") : TEXT("Failed")
		));
}
