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
    
        FVector location = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("CollectibleLocation"));
        

        if (aiController->reached){

          
            FVector collectibleLocation = GetRandomCollectibleLocation(aiController);
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("CollectibleLocation"), collectibleLocation);
            aiController->reached = false;
          

        }
       
       
    }
}


FVector UMyBTService_TryPickUpLocation::GetRandomCollectibleLocation(ASDTAIController* aiController) {

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    FVector collectibleLocation = aiController->GetPawn()->GetActorLocation();
    bool search = true;
    if (foundCollectibles.Num() > 0) {

        while (search)
        {
            int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

            ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
            if (!collectibleActor)
                return collectibleLocation;

            if (!collectibleActor->IsOnCooldown())
            {
                collectibleLocation = collectibleActor->GetActorLocation();
                search = false;
            }

        }
    }

    return collectibleLocation;
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