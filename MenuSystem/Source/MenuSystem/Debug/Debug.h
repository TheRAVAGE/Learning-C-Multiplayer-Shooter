#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"

namespace Debug
{
	//Basic Debugging Functionalities
	// Use this to Log in Output
	static void LogMsg (const FString &Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}
	
	// Use this to Print and Log in Output
	static void PrintMessage(const FString &Message, const FColor &Color = FColor::MakeRandomColor(), bool bLog = false)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
			if (bLog)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
			}
		}
	}
	// Use this to Print and Log constantly changing values
	static void PrintString (const UWorld* WorldContext, const FName Title = NAME_None, const FString &Message = "", const float Duration = 10.0f,const FColor &Color = FColor::MakeRandomColor(), bool bLog = false  )
	{
		if (Title != NAME_None)
		{
			const FString MessageString = FString::Printf(TEXT("%s : %s"), *Title.ToString(), *Message);
			UKismetSystemLibrary::PrintString(WorldContext,MessageString, true,bLog,Color,Duration,Title );
			if (bLog){ LogMsg(MessageString); }
		}
		else
		{
			UKismetSystemLibrary::PrintString(WorldContext,Message, true, bLog, Color, Duration );
			if (bLog){ LogMsg(Message); }
		}
	}
}
