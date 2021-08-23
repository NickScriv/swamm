// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ConfusedMonster.generated.h"

/**
 * 
 */
UCLASS()
class GLITTERGOLD_API UBTTaskNode_ConfusedMonster : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
