// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_TryPlayerLocation.h"

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

UMyBTService_TryPlayerLocation::UMyBTService_TryPlayerLocation() {
	bCreateNodeInstance = true;
}

void UMyBTService_TryPlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController)
    {
        ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        FVector playerLocation = playerCharacter->GetActorLocation();
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(OwnerComp.GetBlackboardComponent()->GetKeyID("PlayerLocation"), playerLocation);
        //OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), playerLocation);
        DrawDebugSphere(aiController->GetPawn()->GetWorld(), playerLocation + FVector(0.f, 0.f, 100.f), 25.0f, 32, FColor::Red);
    }
}