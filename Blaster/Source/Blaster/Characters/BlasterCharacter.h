// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"


struct FInputActionValue;
class UInputAction;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category="Inputs")
	class UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, Category="Inputs")
	UInputAction* MoveInput;
	UPROPERTY(EditAnywhere, Category="Inputs")
	UInputAction* LookInput;
	UPROPERTY(EditAnywhere, Category="Inputs")
	UInputAction* JumpInput;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;
	
	//Input Functions
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleLookInput(const FInputActionValue& Value);
	void HandleJumpStartInput(const FInputActionValue& Value);
	void HandleJumpStopInput(const FInputActionValue& Value);
	
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

public:	

	

};
