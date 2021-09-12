// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_KillPlayer.h"
#include "Monster.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"


EBTNodeResult::Type UBTTaskNode_KillPlayer::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

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
	UE_LOG(LogTemp, Warning, TEXT("check player"));
	if (GetWorld()->LineTraceSingleByChannel(hit, start, player->GetCapsuleComponent()->GetComponentLocation(), ECC_Visibility, qParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit something"));
		if (Cast<AMainCharacter>(hit.Actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Kill Player"));
			//monster->StopMonsterSounds();
			monster->KillPlayer();
		}
	}
	//DrawDebugLine(GetWorld(), start, player->GetCapsuleComponent()->GetComponentLocation(), FColor::Yellow, false, 2.f);
	

	return EBTNodeResult::Succeeded;
}