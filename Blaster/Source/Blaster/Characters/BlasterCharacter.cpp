// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Characters/BlasterCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"

#include "Blaster/Debug/Debug.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


ABlasterCharacter::ABlasterCharacter()
{
	bReplicates = true;
	SetReplicateMovement(true);
	GetMesh()->SetIsReplicated(true);

	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Debug::PrintMessage("BeginPlay called");
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
			if (Subsystem)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	
}

// ~ Input Binding and Handling - Starts here
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Debug::PrintMessage("SetupPlayerInputComponent called");
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInput, ETriggerEvent::Triggered, this, &ABlasterCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(LookInput, ETriggerEvent::Triggered, this, &ABlasterCharacter::HandleLookInput);
		
		EnhancedInputComponent->BindAction(JumpInput, ETriggerEvent::Started, this, &ABlasterCharacter::HandleJumpStartInput);
		EnhancedInputComponent->BindAction(JumpInput, ETriggerEvent::Completed, this, &ABlasterCharacter::HandleJumpStopInput);
	}

}

void ABlasterCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	Debug::PrintString(GetWorld(), "Move Input", FString::Printf(TEXT("X: %f, Y: %f"), MovementVector.X, MovementVector.Y), 10.0f, FColor::Yellow);
	DoMove(MovementVector.X, MovementVector.Y);
}

void ABlasterCharacter::HandleLookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	Debug::PrintString(GetWorld(), "Look Input", FString::Printf(TEXT("X: %f, Y: %f"), LookAxisVector.X, LookAxisVector.Y), 10.0f, FColor::Blue);
	DoLook(LookAxisVector.X, LookAxisVector.Y);
	
}

void ABlasterCharacter::HandleJumpStartInput(const FInputActionValue& Value)
{
	Debug::PrintString(GetWorld(), "Jump Input", "Pressed", 10.0f, FColor::Green);
	Jump();
}

void ABlasterCharacter::HandleJumpStopInput(const FInputActionValue& Value)
{
	Debug::PrintString(GetWorld(), "Jump Input", "Released", 10.0f, FColor::Red);
	StopJumping();
}

void ABlasterCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ABlasterCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController())
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(-Pitch);
	}
}

// ~ Input Binding and Handling - Ends here


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}