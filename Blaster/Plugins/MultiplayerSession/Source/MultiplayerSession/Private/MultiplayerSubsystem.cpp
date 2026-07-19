// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

#define Print(text) if(GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Purple, text); }

UMultiplayerSubsystem::UMultiplayerSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		Print(FString::Printf(TEXT("UMultiplayerSubsystem :: Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString()));
	}
}

void UMultiplayerSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	Print("UMultiplayerSubsystem :: CreateSession : Create Session Triggered");
	
	if(!SessionInterface.IsValid())
	{
		Print("UMultiplayerSubsystem :: CreateSession : Session Interface Not Valid");
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}
	
	auto ExistingSessions = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSessions!=nullptr )
	{
		// try commenting out the below line to see if it works without destroying the session first
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		DestroySession();
		// SessionInterface->DestroySession(NAME_GameSession);
		return;
	}
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	// LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		Print("UMultiplayerSubsystem :: CreateSession : Failed to create Session");
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}

void UMultiplayerSubsystem::FindSessions(int32 MaxSearchResults)
{
	Print("UMultiplayerSubsystem :: FindSessions : Find Sessions Triggered");
	if (!SessionInterface.IsValid())
	{
		Print("UMultiplayerSubsystem :: FindSessions : Session Interface Not Valid");
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		Print("UMultiplayerSubsystem :: FindSessions : Failed to find Sessions");
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	
}

void UMultiplayerSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	Print("UMultiplayerSubsystem :: JoinSession : Join Session Triggered");
	
	if (!SessionInterface.IsValid())
	{
		Print("UMultiplayerSubsystem :: JoinSession : Session Interface Not Valid");
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		Print("UMultiplayerSubsystem :: JoinSession : Failed to join Session");
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		Print("MultiplayerSubsystem :: DestroySession : Session Interface Not Valid");
		MultiplayerOnDestroySessionComplete.Broadcast( false);
		return;
	}
	
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		Print("MultiplayerSubsystem :: DestroySession : Failed to destroy Session");
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast( false);
	}
}

void UMultiplayerSubsystem::StartSession()
{
}

//CallBack Fundctions
void UMultiplayerSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
	Print(FString::Printf(
		TEXT("UMultiplayerSubsystem :: OnCreateSessionComplete : Session %s, bWasSuccessful: %s"), 
		*SessionName.ToString(), 
		bWasSuccessful ? TEXT("True") : TEXT("False")
		));
}

void UMultiplayerSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	Print(FString::Printf(
		TEXT("UMultiplayerSubsystem :: OnFindSessionsComplete with %d Sessions :  bWasSuccessful: %s"), 
		LastSessionSearch->SearchResults.Num(), 
		bWasSuccessful ? TEXT("True") : TEXT("False")
		));
	
	MultiplayerOnFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	
}

void UMultiplayerSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	Print(FString::Printf(
		TEXT("UMultiplayerSubsystem :: OnJoinSessionComplete : Session %s, Result: %s"), 
		*SessionName.ToString(), 
		LexToString(Result)
		));
	
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	Print(FString::Printf(
		TEXT("UMultiplayerSubsystem :: OnDestroySessionComplete : Session %s, bWasSuccessful: %s"), 
		*SessionName.ToString(), 
		bWasSuccessful ? TEXT("True") : TEXT("False")
		));
	
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	Print(FString::Printf(
		TEXT("UMultiplayerSubsystem :: OnStartSessionComplete : Session %s, bWasSuccessful: %s"),
		*SessionName.ToString(), 
		bWasSuccessful ? TEXT("True") : TEXT("False")
		));
}
