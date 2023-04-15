// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToCollectible.h"
#include "SDTUtils.h"
#include <EngineMinimal.h>
#include "SDTFleeLocation.h"
#include "EngineUtils.h"
#include "SDTPathFollowingComponent.h"
#include <SoftDesignTraining/SDTCollectible.h>
#include "NavigationSystem.h"
#include <chrono>
#include "DrawDebugHelpers.h"

EBTNodeResult::Type UMyBTTask_MoveToCollectible::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
        if (aiController->AtJumpSegment)
            return EBTNodeResult::Failed;
        auto startTime = std::chrono::system_clock::now();
        FVector collectibleLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("CollectibleLocation"));
      
        if ((collectibleLocation - aiController->GetPawn()->GetActorLocation()).Size() <= 100.f) {
            aiController->reached = true;
        }
        else {

            aiController->MoveToLocation(collectibleLocation);
        }
        auto stopTime = std::chrono::system_clock::now();
        long duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

