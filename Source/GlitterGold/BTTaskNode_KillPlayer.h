// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_KillPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GLITTERGOLD_API UBTTaskNode_KillPlayer : public UBTTaskNode
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		float killDistance;

	UBTTaskNode_KillPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
