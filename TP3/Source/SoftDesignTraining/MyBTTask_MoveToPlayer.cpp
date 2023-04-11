// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToPlayer.h"
#include <SoftDesignTraining/SDTAIController.h>


EBTNodeResult::Type UMyBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		aiController->MoveToPlayer();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}