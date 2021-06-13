// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TriggerVolume.h"
//#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"

// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	firstEventScare->OnActorBeginOverlap.AddDynamic(this, &AMonster::TriggerFirstEvent);

	passiveEvent = FAkAudioDevice::Get()->PostEvent("Play_Enemy_Passive_Sounds", this);
}

void AMonster::TriggerFirstEvent(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		AMainCharacter* player = Cast<AMainCharacter>(otherActor);

		if (!player)
		{
			UE_LOG(LogTemp, Error, TEXT("Player Not found in trigger"));
			return;
		}

		startAnimation = true;
	}
}

void AMonster::KillPlayer()
{
	AAIController* controller =  UAIBlueprintHelperLibrary::GetAIController(this);

	if (controller)
	{
		AMainCharacter* player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (player)
		{
			player->Died(this);
			controller->SetFocus(player);

			FAkAudioDevice::Get()->PostEvent("Death_Music", this);
			player->StopPlayerSounds();
			FVector playerLookAt = GetActorLocation();
			rotateKill= UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player->GetActorLocation());

			killedPlayer = true;
		}
		controller->GetBrainComponent()->StopLogic("Dead");
	}

	GetCharacterMovement()->DisableMovement();
	

}

void AMonster::StopMonsterSounds()
{
	UE_LOG(LogTemp, Warning, TEXT("stop sounds"));
	UE_LOG(LogTemp, Warning, TEXT("passive: %d"), passiveEvent);
	UE_LOG(LogTemp, Warning, TEXT("caution: %d"), cautionEvent);
	UE_LOG(LogTemp, Warning, TEXT("chase: %d"), chaseEvent);
	FAkAudioDevice::Get()->StopPlayingID(passiveEvent, 300);
	FAkAudioDevice::Get()->StopPlayingID(cautionEvent, 300);
	FAkAudioDevice::Get()->StopPlayingID(chaseEvent, 300);
}

void AMonster::PlayMosnterSoundEvent(FString event)
{
	if (event == "Play_Enemy_Passive_Sounds")
	{
		passiveEvent = FAkAudioDevice::Get()->PostEvent(*event, this);
	}
	else if (event == "Play_Enemy_Caution_Sounds")
	{
		cautionEvent = FAkAudioDevice::Get()->PostEvent(*event, this);
	}
	else
	{
		chaseEvent = FAkAudioDevice::Get()->PostEvent(*event, this);
	}
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (killedPlayer)
	{
		FRotator temp = GetActorRotation();
		FRotator res = FMath::RInterpTo(GetActorRotation(), rotateKill, DeltaTime, killRotTime);
		res.Pitch = temp.Pitch;
		res.Roll = temp.Roll;
		SetActorRotation(res);
	}
	
}

