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
#include <chrono>

UMyBTService_TryDetectPlayer::UMyBTService_TryDetectPlayer() {
	bCreateNodeInstance = true;
}

void UMyBTService_TryDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (!aiController) retunr;
    
    auto startTime = std::chrono::system_clock::now();

    bool isPlayerDetected = aiController->TryDetectPlayer();
    // DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), isPlayerDetected ? TEXT("Player detected") : TEXT("Player not detected"), aiController->GetPawn(), FColor::Blue, 0.4f, false);

    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(OwnerComp.GetBlackboardComponent()->GetKeyID("IsPlayerDetected"), isPlayerDetected);

    auto stopTime = std::chrono::system_clock::now();
    long duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count();
    DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), "Player detection CPU: " + FString::FromInt(duration) + " ms", aiController->GetPawn(), FColor::Blue, 0.f, false);

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


