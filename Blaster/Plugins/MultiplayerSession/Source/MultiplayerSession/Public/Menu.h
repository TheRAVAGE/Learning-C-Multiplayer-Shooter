// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;
class UMultiplayerSubsystem;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(TSoftObjectPtr<UWorld> Lobby, int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll"))) ;
	UFUNCTION(BlueprintCallable)
	void MenuTearDown();
	
protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	
private:
	
	UMultiplayerSubsystem* MultiplayerSubsystem;
	
	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FName LobbyName{TEXT("Lobby")};
	TSoftObjectPtr<UWorld> LobbyLevel{nullptr};
	
	UPROPERTY(meta = (BindWidget))
	UButton* CreateButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	
	UFUNCTION()
	void CreateButtonClicked();
	
	UFUNCTION()
	void JoinButtonClicked();
	
	//
	// Callbacks for custom delegates on the Multiplayer Session Subsystem
	//
	UFUNCTION()
	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnFindSessionComplete( const TArray<FOnlineSessionSearchResult>& SessionResults ,bool bWasSuccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySessionComplete(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSessionComplete(bool bWasSuccessful);
};
