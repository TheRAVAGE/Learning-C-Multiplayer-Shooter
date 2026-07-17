// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"

#include "Debug/Debug.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();
	Debug::PrintString(GetWorld(), "Players In Game : ", FString::Printf(TEXT("%d"), NumberOfPlayer), 60.0f,FColor::Yellow );
	
	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		Debug::PrintMessage(FString::Printf(TEXT("Player %s has joined the lobby."), *PlayerName));
		
	}
}

void ALobbyGameMode::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);
	
	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();
	Debug::PrintString(GetWorld(), "Players In Game : ", FString::Printf(TEXT("%d"), NumberOfPlayer-1), 60.0f,FColor::Yellow );
	
	APlayerState* PlayerState = ExitingPlayer->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		Debug::PrintMessage(FString::Printf(TEXT("Player %s has left the lobby."), *PlayerName));
		
	}
}
