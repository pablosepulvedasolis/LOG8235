// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_TryFleeLocation.h"

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

UMyBTService_TryFleeLocation::UMyBTService_TryFleeLocation() {
    bCreateNodeInstance = true;
}

void UMyBTService_TryFleeLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner());
    if (aiController)
    {
        FVector bestFleeLocation = findBestFleeLocation(aiController);
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("BestFleeLocation"), bestFleeLocation);
        //OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("BestFleeLocation"), bestFleeLocation);
    }
}


FVector UMyBTService_TryFleeLocation::findBestFleeLocation(ASDTAIController* aiController) {

    float bestLocationScore = 0.f;
    FVector bestFleeLocation = aiController->GetPawn()->GetActorLocation();

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return bestFleeLocation;

    for (TActorIterator<ASDTFleeLocation> actorIterator(GetWorld(), ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - aiController->GetPawn()->GetActorLocation();
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - aiController->GetPawn()->GetActorLocation();
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation->GetActorLocation();
            }

            // DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }
    return bestFleeLocation;
}
