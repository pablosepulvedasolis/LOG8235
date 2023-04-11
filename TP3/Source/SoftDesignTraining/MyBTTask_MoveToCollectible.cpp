// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToCollectible.h"
#include <SoftDesignTraining/SDTAIController.h>


EBTNodeResult::Type UMyBTTask_MoveToCollectible::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		aiController->MoveToBestCollectible();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
