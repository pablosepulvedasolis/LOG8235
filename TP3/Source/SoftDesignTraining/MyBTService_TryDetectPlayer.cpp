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

UMyBTService_TryDetectPlayer::UMyBTService_TryDetectPlayer() {
	bCreateNodeInstance = true;
}

void UMyBTService_TryDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		bool isPlayerDetected = aiController->TryDetectPlayer();
		//DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), isPlayerDetected ? TEXT("Player detected") : TEXT("Player not detected"), aiController->GetPawn(), FColor::Blue, 0.4f, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerDetected"), isPlayerDetected);

		//debug
		//bool test = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsPlayerDetected"));
		//DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), test ? TEXT("Player detected") : TEXT("Player not detected"), aiController->GetPawn(), FColor::Blue);

		bool isPlayerBuffed = SDTUtils::IsPlayerPoweredUp(GetWorld());
		//DrawDebugString(GetWorld(), FVector(0.f, 10.f, 10.f), isPlayerBoosted ? TEXT("Player boosted") : TEXT("Player not boosted"), aiController->GetPawn(), FColor::Red, 0.4f, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerBuffed"), isPlayerBuffed);

		ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		FVector playerPosition = playerCharacter->GetActorLocation();
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), playerPosition);

        FVector bestFleePosition = findBestFleeLocation(aiController);
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("BestFleeLocation"), bestFleePosition);

        FVector collectiblePosition = findRandomCollectibleLocation(aiController);
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("CollectibleLocation"), collectiblePosition);




	}
}

FVector UMyBTService_TryDetectPlayer::findBestFleeLocation(ASDTAIController* aiController) {

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    /*if (!playerCharacter)
        return;*/

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

    return bestFleeLocation->GetActorLocation();
}

FVector UMyBTService_TryDetectPlayer::findRandomCollectibleLocation(ASDTAIController* aiController) {

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    FVector collectibleLocation = aiController->GetPawn()->GetActorLocation();
    bool search = true;
    while (search)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        /*if (!collectibleActor)
            return;*/

        if (!collectibleActor->IsOnCooldown())
        {
            collectibleLocation = collectibleActor->GetActorLocation();
            search = false;
        }

    }

    return collectibleLocation;
}

FVector UMyBTService_TryDetectPlayer::findBestCollectibleLocation(ASDTAIController* aiController) {

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
        if (closestSqrCollectibleDistance > sum) {
            
            closestSqrCollectibleDistance = sum;
            closestCollectible = col;
            
        }
    }

    return closestCollectible->GetActorLocation();

}