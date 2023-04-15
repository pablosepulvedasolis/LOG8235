// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_IsPlayerDetected.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include <SoftDesignTraining/SDTAIController.h>

EBTNodeResult::Type UMyBTTask_IsPlayerDetected::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		bool isPlayerDetected = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsPlayerDetected"));
		//DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), isPlayerDetected ? TEXT("Player detected") : TEXT("Player not detected"), aiController->GetPawn(), FColor::Blue, 0.4f, false);
		if (isPlayerDetected)
		{
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}