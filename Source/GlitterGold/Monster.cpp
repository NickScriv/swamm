// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Flashlight.h"
#include "Drawer.h"
#include "KeyActor.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TriggerVolume.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkComponent.h"
#include "Components/CapsuleComponent.h"
#include "GlitterGameModeBase.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "GlitterGameInstance.h"
#include "MonsterAIController.h"


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

	if(firstEventScare)
		firstEventScare->OnActorBeginOverlap.AddUniqueDynamic(this, &AMonster::TriggerFirstEvent);
	else
		UE_LOG(LogTemp, Error, TEXT("AMonster: Trigger not found for scare event in monster!!"));

	mainPlayer = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if(!mainPlayer)
		UE_LOG(LogTemp, Error, TEXT("AMonster: Player not found!!!"));

	gameMode = Cast<AGlitterGameModeBase>(UGameplayStatics::GetGameMode(this));

	gameInstance = Cast<UGlitterGameInstance>(UGameplayStatics::GetGameInstance(this));

	if ((physicsComponent = Cast<UPhysicalAnimationComponent>(GetComponentByClass(UPhysicalAnimationComponent::StaticClass()))) == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Physical animation component in monster is null!!!"));
		return;
	}

	if (GetMesh())
	{
		physicsComponent->SetSkeletalMeshComponent(GetMesh());
	}

	FPhysicalAnimationData data;
	data.OrientationStrength = OrientationStrength;
	data.AngularVelocityStrength = AngularVelocityStrength;
	data.PositionStrength = PositionStrength;
	data.VelocityStrength = VelocityStrength;
	data.bIsLocalSimulation = false;
	data.MaxAngularForce = 0.f;
	data.MaxLinearForce = 0.f;

	physicsComponent->ApplyPhysicalAnimationSettingsBelow(FName("pelvis"), data, false);
	SetPhysicsAnimation(FName("clavicle_l"));
	SetPhysicsAnimation(FName("clavicle_r"));
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), 0.f, false, false);
	//SetPhysicsAnimation(FName("neck_01"));

	const int32 index = FMath::RandRange(0, possibleStartPaths.Num() - 1);

	currentPath = possibleStartPaths[index].pathPoints;
}

void AMonster::TriggerFirstEvent(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		AMainCharacter* player = Cast<AMainCharacter>(otherActor);

		if (!player)
		{
			UE_LOG(LogTemp, Error, TEXT("AMonster: Player Not found in trigger"));
			return;
		}
		FAkAudioDevice::Get()->PostEvent("Play_Enemy_Passive_Sounds", this);
		startAnimation = true;
		firstEventScare->Destroy();
	}
}

float AMonster::ScaleRange(float input, float inputLow, float inputHigh, float outputLow, float outputHigh)
{
	return ((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow;
}

float AMonster::ReverseNumber(float num, float min, float max)
{
	return (max + min) - num;
}

void AMonster::KillMonster(FVector shotDir)
{
	if (gameInstance && gameInstance->monsterKilled)
		return;


	if (gameInstance)
		gameInstance->monsterKilled = true;

	SetPhysicsAnimation(FName("pelvis"));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

	FAkAudioDevice::Get()->SetRTPCValue(*FString("Danger_Warning"), 0.f, 200, mainPlayer);
	FAkAudioDevice::Get()->PostEvent("Player_Kills_Enemy", this);

	if (AMainCharacter* player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		FAkAudioDevice::Get()->PostEvent("Play_Ambient_Music", player);
		FAkAudioDevice::Get()->SetRTPCValue(*FString("Num_of_Keys"), 0, 50, player);
	}

	//GetWorldTimerManager().SetTimer(timerHandleRagdoll, this, &AMonster::EnableRagdoll, ragDollTime, false);
	//EnableRagdoll();

	float dot = FVector::DotProduct(shotDir, GetActorForwardVector());

	if (dot >= 0.4f)
	{
		// Die Forwards
		deathStatus = EDeathStatus::DieForwards;

	}
	else if (dot <= -0.4f)
	{
		// Die Backwards
		deathStatus = EDeathStatus::DieBackwards;

	}
	else
	{
		// Die sideways
		float dotSide = FVector::DotProduct(shotDir, GetActorRightVector());

		if (dotSide < 0)
		{
			// Die left
			deathStatus = EDeathStatus::DieLeft;
		}
		else
		{
			//Die right
			deathStatus = EDeathStatus::DieRight;
		}

	}

	DetachFromControllerPendingDestroy();
}

void AMonster::KillTest()
{
	KillMonster(GetActorForwardVector());
}

void AMonster::EnableRagdoll()
{
	//physicsComponent->ApplyPhysicalAnimationProfileBelow("pelvis", "Death");
	//GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, false);
	//GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), 1.0f, false, false);
	GetMesh()->SetSimulatePhysics(true);
}

void AMonster::SetPhysicsAnimation(FName boneName)
{
	if (GetMesh())
	{
		GetMesh()->SetAllBodiesBelowSimulatePhysics(boneName, true, false);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(boneName, blendPhysics, false, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh in monster not found!!!"));
	}
}

void AMonster::SetKeys()
{
	if (firstKeySpawnLocations.Num() > 0)
	{
		const int32 index = FMath::RandRange(0, firstKeySpawnLocations.Num() - 1);

		firstKey->SetActorTransform(firstKeySpawnLocations[index]);
	}

	if (secondKeySpawnLocations.Num() > 0)
	{
		const int32 index = FMath::RandRange(0, secondKeySpawnLocations.Num() - 1);

		secondKey->SetActorTransform(secondKeySpawnLocations[index]);
	}

	if (thirdKeySpawnLocations.Num() > 0)
	{
		const int32 index = FMath::RandRange(0, thirdKeySpawnLocations.Num() - 1);

		thirdKey->SetActorTransform(thirdKeySpawnLocations[index]);
	}
}

void AMonster::SetFlashlight()
{
	if (flashlightDrawer && ShouldHappen(35))
	{
		flashlight->AttachToActor(flashlightDrawer, FAttachmentTransformRules::SnapToTargetIncludingScale);
		flashlight->SetActorTransform(flashlightDrawerLocation);
	}
	else if(flashlightSpawnLocations.Num() > 0)
	{
		const int32 index = FMath::RandRange(0, flashlightSpawnLocations.Num() - 1);
		flashlight->SetActorTransform(flashlightSpawnLocations[index]);
	}
}

void AMonster::PlayDeathSound(AMainCharacter* player)
{
	FAkAudioDevice::Get()->PostEvent("Death_Music", player);
	FAkAudioDevice::Get()->PostEvent("Death_Enemy_Sounds", this);
}

void AMonster::ChangeCurrentPath(TArray<APathPoint*> path)
{
	// using random values
	if (ShouldHappen(50))
	{
		pathDirection = 1;
	}
	else
	{
		pathDirection = -1;
	}

	currentPath = path;

	currentPathIndex = FMath::RandRange(0, currentPath.Num() - 1);

	pathEnding = static_cast<EPathEnding>(FMath::RandRange(1, 2));

	//const UEnum* pathEndingEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPathEnding"));
	/*UE_LOG(LogTemp, Warning, TEXT("Path ending state: %s")
		, *(pathEndingEnum ? pathEndingEnum->GetEnumName(pathEnding) : TEXT("<Invalid Enum>")));*/

	AMonsterAIController* monsterController = Cast<AMonsterAIController>(GetController());

	if (monsterController)
	{
		monsterController->TriggerPatrolAbort();
	}
}

void AMonster::TogglePhysics(bool physicsOn)
{
	isPhysicsOn = physicsOn;
	if (physicsOn)
	{
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), blendPhysics, false, false);
		//UE_LOG(LogTemp, Warning, TEXT("Physics on"));
	}
	else
	{
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), blendPhysicsLight, false, false);
		//UE_LOG(LogTemp, Warning, TEXT("Physics off"));
	}

	
}

bool AMonster::ShouldHappen(int32 percentage)
{
	return (FMath::RandRange(1, 100 / percentage) == 1 ? true : false);
}

void AMonster::KillPlayer(AMainCharacter* player)
{
	AAIController* controller =  UAIBlueprintHelperLibrary::GetAIController(this);
	
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), false, false);
	if (controller)
	{
		player->Died(this);
		controller->SetFocus(player);
		killTimerDel.BindUFunction(this, FName("PlayDeathSound"), player);
		GetWorld()->GetTimerManager().SetTimer(killTimerHandle, killTimerDel, deathSoundTimer, false);
		player->StopPlayerSounds();
		FVector playerLookAt = GetActorLocation();
		rotateKill= UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player->GetActorLocation());

		killedPlayer = true;

		controller->GetBrainComponent()->StopLogic("Dead");
	}

	GetCharacterMovement()->DisableMovement();

	if(gameMode)
		gameMode->playerKilled = true;
	
	DetachFromControllerPendingDestroy();
}

void AMonster::StopMonsterSounds()
{
	/*FAkAudioDevice::Get()->StopPlayingID(passiveEvent, 300);
	FAkAudioDevice::Get()->StopPlayingID(cautionEvent, 300);
	FAkAudioDevice::Get()->StopPlayingID(chaseEvent, 300);*/
}

void AMonster::PlayMosnterSoundEvent(FString event)
{
	if (event == "Play_Enemy_Passive_Sounds")
	{
		FAkAudioDevice::Get()->PostEvent(*event, this);
		gameMode->monsterInCaution = 0.f;
	}
	else if (event == "Play_Enemy_Caution_Sounds")
	{
		FAkAudioDevice::Get()->PostEvent(*event, this);
		gameMode->monsterInCaution = 1.f;
	}
	else
	{
		FAkAudioDevice::Get()->PostEvent(*event, this);
		gameMode->monsterInCaution = 1.f;
	}
}

void AMonster::TakeMonsterDamage(float damage, const FVector& shotDir)
{
	if (gameInstance->monsterKilled)
		return;

	health -= damage;
	if(health <= 0.f)
		KillMonster(shotDir);
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult OutHit;
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), GetActorRotation(), true, OutHit);
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), GetActorRotation(), true, OutHit);

	FHitResult hit;
	FCollisionQueryParams qParams;
	qParams.AddIgnoredActor(this);

	if (!gameInstance->monsterKilled && GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), mainPlayer->GetCapsuleComponent()->GetComponentLocation(), ECC_Visibility, qParams))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Name of coll: %s"), *hit.Actor->GetName());
		AMainCharacter* player = Cast<AMainCharacter>(hit.Actor);
		if (player)
		{
			//UE_LOG(LogTemp, Warning, TEXT("danger"));
			inLineOfPlayer = true;
			float dist = hit.Distance;
			dist = FMath::Clamp(dist, minDistanceAmbience, maxDistanceAmbience);
			dist = ReverseNumber(ScaleRange(dist, minDistanceAmbience, maxDistanceAmbience, 0.0f, 100.f), 0.0f, 100.f);
			FAkAudioDevice::Get()->SetRTPCValue(*FString("Danger_Warning"), dist, 0, player);
		}
		else if(inLineOfPlayer)
		{
			//UE_LOG(LogTemp, Warning, TEXT("no danger"));
			inLineOfPlayer = false;
			FAkAudioDevice::Get()->SetRTPCValue(*FString("Danger_Warning"), 0, 0, UGameplayStatics::GetPlayerCharacter(this, 0));
		}
	}


	if (killedPlayer)
	{
		FRotator temp = GetActorRotation();
		FRotator res = FMath::RInterpTo(GetActorRotation(), rotateKill, DeltaTime, killRotTime);
		res.Pitch = temp.Pitch;
		res.Roll = temp.Roll;
		SetActorRotation(res);
	}

	
}

