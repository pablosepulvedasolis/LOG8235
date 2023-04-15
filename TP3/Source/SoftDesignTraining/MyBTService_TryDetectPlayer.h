// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include <SoftDesignTraining/SDTAIController.h>

#include "MyBTService_TryDetectPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTService_TryDetectPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
	public:
		UMyBTService_TryDetectPlayer();

		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
		
};
