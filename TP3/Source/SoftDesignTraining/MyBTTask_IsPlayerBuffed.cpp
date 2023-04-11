// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_IsPlayerBuffed.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include <SoftDesignTraining/SDTAIController.h>
EBTNodeResult::Type UMyBTTask_IsPlayerBuffed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		bool isPlayerBuffed = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsPlayerBuffed"));
		DrawDebugString(GetWorld(), FVector(100.f, 0.f, 10.f), isPlayerBuffed ? TEXT("Player powered up") : TEXT("Player not powered up"), aiController->GetPawn(), FColor::Blue, 0.4f, false);

		if (isPlayerBuffed)
		{
			DrawDebugString(GetWorld(), FVector(100.f, 0.f, 10.f), TEXT("Player powered up"), aiController->GetPawn(), FColor::Blue, 0.4f, false);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}