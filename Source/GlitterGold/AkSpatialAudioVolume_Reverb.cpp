// Fill out your copyright notice in the Description page of Project Settings.


#include "AkSpatialAudioVolume_Reverb.h"
#include "GlitterGameModeBase.h"
#include "Monster.h"
#include "MainCharacter.h"
#include "PointInMonster.h"
#include "PointInPlayer.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

class APointInPlayer;

AAkSpatialAudioVolume_Reverb::AAkSpatialAudioVolume_Reverb()
{
}

// Called when the game starts or when spawned
void AAkSpatialAudioVolume_Reverb::BeginPlay()
{
	Super::BeginPlay();

	// listen to event
	OnActorBeginOverlap.AddUniqueDynamic(this, &AAkSpatialAudioVolume_Reverb::OnOverlapBegin);
}

void AAkSpatialAudioVolume_Reverb::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		APointInPlayer* point = Cast<APointInPlayer>(otherActor);
		if (point)
		{
			if (AMonster* monster = Cast<AMonster>(point->GetParentActor()))
			{
				// Monster stepped in
				//UE_LOG(LogTemp, Warning, TEXT("PointInMonster detected!"));
				FAkAudioDevice::Get()->SetRTPCValue(*FString("Enemy_Footsteps_Surface_Type"), footStepSurface, 0, monster);

			}
			else if (AMainCharacter* player = Cast<AMainCharacter>(point->GetParentActor()))
			{

				if (IsValid(lightBlocker))
				{
					lightBlocker->Destroy();
					lightBlocker = nullptr;
				}

				AGlitterGameModeBase* gameMode = Cast<AGlitterGameModeBase>(UGameplayStatics::GetGameMode(this));
				gameMode->queuedMusic = musicName;
				//UE_LOG(LogTemp, Warning, TEXT("Change footstep surface: %i"), footStepSurface);
				FAkAudioDevice::Get()->SetRTPCValue(*FString("Footsteps_Surface_Type"), footStepSurface, 0, player);
				//UE_LOG(LogTemp, Warning, TEXT("PointInPlayer detected!"));

				if (FMath::IsNearlyEqual(gameMode->monsterInCaution, 0.0f, 0.2f))
				{
					FAkAudioDevice::Get()->PostEvent(gameMode->queuedMusic, player);
				}
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player nor monster entered the reverb trigger %s"), *otherActor->GetName());
			}
		}
		
	}
	
}

