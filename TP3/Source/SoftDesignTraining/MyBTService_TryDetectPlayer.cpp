// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_TryDetectPlayer.h"

#include <SoftDesignTraining/SDTAIController.h>
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "DrawDebugHelpers.h"
#include "SDTUtils.h"
#include <EngineMinimal.h>
#include "SDTFleeLocation.h"
#include "EngineUtils.h"
#include <SoftDesignTraining/SDTCollectible.h>
#include "NavigationSystem.h"
#include <SoftDesignTraining/AiAgentGroupManager.h>

UMyBTService_TryDetectPlayer::UMyBTService_TryDetectPlayer() {
	bCreateNodeInstance = true;
}

void UMyBTService_TryDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController)
    {
        bool isPlayerDetected = aiController->TryDetectPlayer();
        //DrawDebugString(GetWorld(), FVector(100.f, 0.f, 10.f), isPlayerDetected ? TEXT("Try player detected") : TEXT("Try player not detected"), aiController->GetPawn(), FColor::Blue, 0.4f, false);

        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(OwnerComp.GetBlackboardComponent()->GetKeyID("IsPlayerDetected"), isPlayerDetected);

        bool isPlayerBuffed = SDTUtils::IsPlayerPoweredUp(GetWorld());
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(OwnerComp.GetBlackboardComponent()->GetKeyID("IsPlayerBuffed"), isPlayerBuffed);

        // AI group manager
        AiAgentGroupManager* aiManagerInstance = AiAgentGroupManager::GetInstance();
        if (isPlayerDetected) 
        {
            aiManagerInstance->RegisterAIAgent(aiController);
            aiManagerInstance->DrawIndicatorSphere(aiController);
        }
        else 
        {
            aiManagerInstance->GetInstance()->UnregisterAIAgent(aiController);
        }
        //aiManagerInstance->DrawIndicatorSphere();
	}
}