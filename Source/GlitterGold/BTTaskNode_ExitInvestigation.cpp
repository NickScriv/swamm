// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_ExitInvestigation.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskNode_ExitInvestigation::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(bbKey_Investigate.SelectedKeyName, 0);
	//UE_LOG(LogTemp, Warning, TEXT("Ivestingate false"));
	return EBTNodeResult::Succeeded;


}

