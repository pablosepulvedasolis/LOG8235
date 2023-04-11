// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <SoftDesignTraining/SDTAIController.h>
#include "AIController.h"

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_MoveToCollectible.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTTask_MoveToCollectible : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	public:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
		FVector GetBestCollectibleLocation(ASDTAIController* aiController);
};
