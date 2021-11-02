// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_KillPlayer.h"
#include "Monster.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTaskNode_KillPlayer::UBTTaskNode_KillPlayer()
{
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTaskNode_KillPlayer::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (!OwnerComp.GetAIOwner())
		return EBTNodeResult::Failed;

	AMonster* monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (!monster)
		return EBTNodeResult::Failed;

	AMainCharacter* player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (!player)
		return EBTNodeResult::Failed;

	FCollisionQueryParams qParams;

	FHitResult hit;
	FVector start = monster->GetActorLocation();
	qParams.AddIgnoredActor(monster);
	start.Z += 130.f;
	//UE_LOG(LogTemp, Warning, TEXT("check player"));
	if (GetWorld()->LineTraceSingleByChannel(hit, start, player->GetCapsuleComponent()->GetComponentLocation(), ECC_Visibility, qParams))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Hit something"));
		if (Cast<AMainCharacter>(hit.Actor) && hit.Distance <= killDistance)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Kill Player"));
			//monster->StopMonsterSounds();
			monster->KillPlayer();
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("check player2"));
	

	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_KillPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	return;
	//UE_LOG(LogTemp, Warning, TEXT("wow"));
	if (!OwnerComp.GetAIOwner())
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	AMonster* monster = Cast<AMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (!monster)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);



	monster->KillPlayer();
}