// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToCollectible.h"
#include "SDTUtils.h"
#include <EngineMinimal.h>
#include "SDTFleeLocation.h"
#include "EngineUtils.h"
#include "SDTPathFollowingComponent.h"
#include <SoftDesignTraining/SDTCollectible.h>
#include "NavigationSystem.h"

EBTNodeResult::Type UMyBTTask_MoveToCollectible::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
        if (aiController->AtJumpSegment)
            return EBTNodeResult::Failed;
        FVector collectibleLocation = GetBestCollectibleLocation(aiController);
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("CollectibleLocation"), collectibleLocation);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

FVector UMyBTTask_MoveToCollectible::GetBestCollectibleLocation(ASDTAIController* aiController)
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