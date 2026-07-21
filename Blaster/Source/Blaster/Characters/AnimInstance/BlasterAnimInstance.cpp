// Fill out your copyright notice in the Description page of Project Settings.


#include "Blaster/Characters/AnimInstance/BlasterAnimInstance.h"

#include "Blaster/Characters/BlasterCharacter.h"
#include "Blaster/Debug/Debug.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (!BlasterCharacter) return;
	
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	
	Debug::PrintString(
		GetWorld(), 
		"AnimInstance Update", 
		FString::Printf(
			TEXT("Speed: %f, InAir: %s, Accelerating: %s"), 
			Speed, 
			bIsInAir ? TEXT("True") : TEXT("False"), 
			bIsAccelerating ? TEXT("True") : TEXT("False")
			), 
		10.f, 
		FColor::Cyan);
}
