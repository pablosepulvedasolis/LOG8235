// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToFlee.h"

#include "SoftDesignTraining.h"
#include "SDTFleeLocation.h"
#include "Kismet/KismetMathLibrary.h"
#include "SDTUtils.h"
#include "EngineUtils.h"
#include <chrono>
#include "DrawDebugHelpers.h"
#include <SoftDesignTraining/SDTAIController.h>

EBTNodeResult::Type UMyBTTask_MoveToFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
        auto startTime = std::chrono::system_clock::now();

		FVector fleeLocation = GetFleeLocation(aiController);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("BestFleeLocation"), fleeLocation);
        aiController->MoveTo(fleeLocation);
        auto stopTime = std::chrono::system_clock::now();
        long duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime).count();
        DrawDebugString(GetWorld(), FVector(100.f, 0.f, 10.f), "Flee calculation CPU: " + FString::FromInt(duration) + " ms", aiController->GetPawn(), FColor::Red, 1.f, false);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

FVector UMyBTTask_MoveToFlee::GetFleeLocation(ASDTAIController* aiController)
{
    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (playerCharacter) 
    {
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
                    bestFleeLocation = fleeLocation;
                }

                DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
            }
        }

        if (bestFleeLocation)
        {
            return bestFleeLocation->GetActorLocation();
        }
    }
    return FVector();
}
