// Copyright Epic Games, Inc. All Rights Reserved.

#include "MenuSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MenuSystem.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

#define Log(x) { GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, x); }

AMenuSystemCharacter::AMenuSystemCharacter()
// OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
// OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
// OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	// IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	// if (OnlineSubsystem)
	// {
	// 	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	// 	if (GEngine) {
	// 		GEngine->AddOnScreenDebugMessage(
	// 			-1,
	// 			15.f,
	// 			FColor::Blue,
	// 			FString::Printf(TEXT("Found Online Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString()
	// 			));
	// 	}
	// }
}

void AMenuSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMenuSystemCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMenuSystemCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMenuSystemCharacter::Look);
	}
	else
	{
		UE_LOG(LogMenuSystem, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMenuSystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMenuSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMenuSystemCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMenuSystemCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMenuSystemCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMenuSystemCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

// void AMenuSystemCharacter::CreateSession()
// {w
// 	// Called when pressing the 1 key
// 	if (!OnlineSessionInterface.IsValid())
// 	{
// 		return;
// 	}
//
// 	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
// 	if (ExistingSession != nullptr)
// 	{
// 		OnlineSessionInterface->DestroySession(NAME_GameSession);
// 	}
//
// 	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
//
// 	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
// 	SessionSettings->bIsLANMatch = false;
// 	SessionSettings->NumPublicConnections = 4;
// 	SessionSettings->bAllowJoinInProgress = true;
// 	SessionSettings->bAllowJoinViaPresence = true;
// 	SessionSettings->bShouldAdvertise = true;
// 	SessionSettings->bUsesPresence = true;
// 	SessionSettings->bUseLobbiesIfAvailable = true;
// 	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
// 	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
// 	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
// }
//
// void AMenuSystemCharacter::JoinSession()
// {
// 	// Find game sessions
// 	if (!OnlineSessionInterface.IsValid())
// 	{
// 		return;
// 	}
// 	
// 	Log(FString("2 key pressed..."));
//
// 	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
//
// 	SessionSearch = MakeShareable(new FOnlineSessionSearch());
// 	SessionSearch->MaxSearchResults = 10000;
// 	SessionSearch->bIsLanQuery = false;
// 	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
//
// 	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
// 	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
// }
//
// void AMenuSystemCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
// {
// 	if (bWasSuccessful)
// 	{
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(
// 				-1,
// 				15.f,
// 				FColor::Blue,
// 				FString::Printf(TEXT("Created session: %s"), *SessionName.ToString())
// 			);
// 		}
//
// 		UWorld* World = GetWorld();
// 		if (World)
// 		{
// 			// UGameplayStatics::OpenLevel(World, FName("/Game/Maps/Lobby"), true, "listen");
// 			FString ServerURL = "/Game/Maps/Lobby";
// 			ServerURL.Append("?listen");
// 			World->ServerTravel(ServerURL);
// 			// World->ServerTravel(FString("/Game/Maps/Lobby?listen"));
// 			Log(FString("Map Added as Listen Server"));
// 		}
// 	}
// 	else
// 	{
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(
// 				-1,
// 				15.f,
// 				FColor::Red,
// 				FString(TEXT("Failed to create session!"))
// 			);
// 		}
// 	}
// }
//
// void AMenuSystemCharacter::OnFindSessionComplete(bool bWasSuccessful)
// {
// 	if (!OnlineSessionInterface.IsValid())
// 	{
// 		return;
// 	}
// 	for (auto Result : SessionSearch->SearchResults)
// 	{
// 		FString Id = Result.GetSessionIdStr();
// 		FString User = Result.Session.OwningUserName;
// 		FString MatchType;
// 		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(
// 				-1,
// 				15.f,
// 				FColor::Cyan,
// 				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
// 			);
// 		}
// 		if (MatchType == FString("FreeForAll"))
// 		{
// 			if (GEngine)
// 			{
// 				GEngine->AddOnScreenDebugMessage(
// 					-1,
// 					15.f,
// 					FColor::Cyan,
// 					FString::Printf(TEXT("Joining Match Type: %s"), *MatchType)
// 				);
// 			}
// 			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
// 			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
// 			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
// 		}
// 	}
// }
//
// void AMenuSystemCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
// {
// 	if (!OnlineSessionInterface.IsValid())
// 	{
// 		return;
// 	}
// 	Log(FString("Join Session Complete."));
// 	FString Address;
// 	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
// 	{
// 		if (GEngine)
// 		{
// 			GEngine->AddOnScreenDebugMessage(
// 				-1,
// 				15.f,
// 				FColor::Yellow,
// 				FString::Printf(TEXT("Connect string: %s"), *Address)
// 			);
// 		}
//
// 		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
// 		if (PlayerController)
// 		{
// 			Log(FString("Initiating Client Travel."));
// 			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
// 		}
// 	}
// }
