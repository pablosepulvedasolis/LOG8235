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
    if (!aiController)
        return;

 
    bool isPlayerDetected = aiController->TryDetectPlayer();
        //bool isPlayerDetected = IsVisible(pawn, playerCharacter);

    OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerDetected"), isPlayerDetected);

    bool isPlayerBuffed = SDTUtils::IsPlayerPoweredUp(GetWorld());
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerBuffed"), isPlayerBuffed);


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


