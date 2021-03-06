// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <GlitterGold/GlitterStructs.h>
#include "Monster.generated.h"


UENUM(BlueprintType)
enum EDeathStatus
{
	DieBackwards    UMETA(DisplayName = "Die Backwards"),
	DieForwards    UMETA(DisplayName = "Die Forwards"),
	DieLeft   UMETA(DisplayName = "Die Left"),
	DieRight    UMETA(DisplayName = "Die Right"),
	Alive    UMETA(DisplayName = "Alive")
};


UENUM()
enum EPathEnding
{
	Nothing    UMETA(DisplayName = "Nothing"),
	PatrolBack   UMETA(DisplayName = "Patrol Back"),
	Loop   UMETA(DisplayName = "Loop")
};

UENUM(BlueprintType)
enum class MonsterStatus : uint8 {
	Patrolling UMETA(DisplayName = "Patrolling"),
	Searching UMETA(DisplayName = "Searching"),
	Chasing UMETA(DisplayName = "Chasing")
};

UCLASS()
class GLITTERGOLD_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

	UPROPERTY(EditAnywhere)
		TArray<FPathPointArray> possibleStartPaths;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<APathPoint*> currentPath;

	UPROPERTY(EditAnywhere)
		TEnumAsByte <EPathEnding>  pathEnding;

	UPROPERTY(EditAnywhere)
		float deathSoundTimer;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte <EDeathStatus>  deathStatus = EDeathStatus::Alive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 currentPathIndex = 0;

	UPROPERTY(EditAnywhere)
	float killRotTime = 10.f;

	UPROPERTY(BlueprintReadOnly)
	bool killedPlayer = false;

	UPROPERTY(EditAnywhere)
	int32 pathDirection = 1;

	UPROPERTY(BlueprintReadOnly)
	bool startAnimation = false;

	UPROPERTY(EditAnywhere)
		float minDistanceAmbience;

	UPROPERTY(EditAnywhere)
		float maxDistanceAmbience;

	UPROPERTY(EditAnywhere)
		float OrientationStrength;

	UPROPERTY(EditAnywhere)
		float AngularVelocityStrength;

	UPROPERTY(EditAnywhere)
		float PositionStrength;

	UPROPERTY(EditAnywhere)
		float VelocityStrength;

	UPROPERTY(BlueprintReadOnly)
		bool isPhysicsOn = false;

	UPROPERTY()
	class UPhysicalAnimationComponent* physicsComponent;

	void KillTest();

	void KillPlayer(class AMainCharacter* player);

	void StopMonsterSounds();

	void PlayMosnterSoundEvent(FString event);

	void TakeMonsterDamage(float damage, const FVector& shotDir);

	void SetKeys();

	void SetFlashlight();

	void ChangeCurrentPath(TArray<APathPoint*> path);

	UFUNCTION(BlueprintCallable)
	void TogglePhysics(bool physicsOn);

	UFUNCTION(BlueprintImplementableEvent)
		void TracePath();

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	FRotator rotateKill;

	FTimerHandle timerHandleRagdoll;

	UPROPERTY(EditAnywhere)
	float health;

	UPROPERTY(EditAnywhere)
		float time;

	UPROPERTY(EditAnywhere)
		float ragDollTime;

	UPROPERTY()
		class UCapsuleComponent* pelvis;

	/*UPROPERTY(EditAnywhere)
		float ragDollTime;*/

	UPROPERTY()
	class AGlitterGameModeBase* gameMode;

	UPROPERTY()
	class UGlitterGameInstance* gameInstance;

	bool inLineOfPlayer = false;

	UPROPERTY(EditAnywhere)
	class ATriggerVolume* firstEventScare;

	UPROPERTY()
		class AMainCharacter* mainPlayer;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		class AKeyActor* firstKey;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		class AKeyActor* secondKey;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		class AKeyActor* thirdKey;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		class AFlashlight* flashlight;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		class ADrawer* flashlightDrawer;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		FTransform flashlightDrawerLocation;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<FTransform> firstKeySpawnLocations;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<FTransform> secondKeySpawnLocations;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<FTransform> thirdKeySpawnLocations;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TArray<FTransform> flashlightSpawnLocations;

	FTimerDelegate killTimerDel;

	FTimerHandle killTimerHandle;

	UPROPERTY(EditAnywhere)
		float blendPhysics;

	UPROPERTY(EditAnywhere)
		float blendPhysicsLight;

	UFUNCTION()
	void TriggerFirstEvent(class AActor* overlappedActor, class AActor* otherActor);

	float ScaleRange(float input, float inputLow, float inputHigh, float outputLow, float outputHigh);

	float ReverseNumber(float num, float min, float max);

	void KillMonster(FVector shotDir);

	void EnableRagdoll();

	void SetPhysicsAnimation(FName boneName);

	bool ShouldHappen(int32 percentage);

	UFUNCTION()
		void PlayDeathSound(AMainCharacter* player);
};






