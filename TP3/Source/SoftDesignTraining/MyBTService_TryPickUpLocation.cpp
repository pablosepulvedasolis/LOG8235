// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_TryPickUpLocation.h"

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

UMyBTService_TryPickUpLocation::UMyBTService_TryPickUpLocation() {
    bCreateNodeInstance = true;
}

void UMyBTService_TryPickUpLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController)
    {
        FVector collectibleLocation = findBestCollectibleLocation(aiController);
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(OwnerComp.GetBlackboardComponent()->GetKeyID("CollectibleLocation"), collectibleLocation);
        //OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("CollectibleLocation"), collectibleLocation);
    }
}

FVector UMyBTService_TryPickUpLocation::findBestCollectibleLocation(ASDTAIController* aiController) 
{
    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

    for (AActor* collectible : foundCollectibles)
    {
        ASDTCollectible* col = Cast<ASDTCollectible>(collectible);
        if (col->IsOnCooldown()) continue;

        UNavigationPath* path = navSys->FindPathToLocationSynchronously(GetWorld(), aiController->GetPawn()->GetActorLocation(), collectible->GetActorLocation());

        double sum = 0;
        FVector  prev;
        for (FVector point : path->PathPoints)
        {
            sum += (point - prev).Size();
            prev = point;
        }
        if (closestSqrCollectibleDistance > sum) 
        {
            closestSqrCollectibleDistance = sum;
            closestCollectible = col;
        }
    }
    return closestCollectible->GetActorLocation();
}