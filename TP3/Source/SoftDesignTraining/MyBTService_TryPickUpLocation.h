// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <SoftDesignTraining/SDTAIController.h>

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MyBTService_TryPickUpLocation.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API UMyBTService_TryPickUpLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	public:
		UMyBTService_TryPickUpLocation();

		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
		FVector findBestCollectibleLocation(ASDTAIController* aiController);
		FVector GetRandomCollectibleLocation(ASDTAIController* aiController);
};
