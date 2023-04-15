// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToCollectible.h"
#include "SDTUtils.h"
#include <EngineMinimal.h>
#include "SDTFleeLocation.h"
#include "EngineUtils.h"
#include "SDTPathFollowingComponent.h"
#include <SoftDesignTraining/SDTCollectible.h>
#include "NavigationSystem.h"

EBTNodeResult::Type UMyBTTask_MoveToCollectible::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
        if (aiController->AtJumpSegment)
            return EBTNodeResult::Failed;
        FVector collectibleLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("CollectibleLocation"));
      
        if ((collectibleLocation - aiController->GetPawn()->GetActorLocation()).Size() <= 100.f) {
            aiController->reached = true;
        }
        else {

            aiController->MoveToLocation(collectibleLocation);
        }

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

