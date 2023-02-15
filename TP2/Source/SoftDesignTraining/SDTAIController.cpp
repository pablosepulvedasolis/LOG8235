// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTCollectible.h"
#include "SDTFleeLocation.h"
#include "SDTPathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
//#include "UnrealMathUtility.h"
#include "SDTUtils.h"
#include "EngineUtils.h"

#include "NavigationSystem.h"
#include "float.h"
#include "AI/Navigation/NavigationTypes.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    //Move to target depending on current behavior

    UNavigationPath* shortestPath = FindClosestCollectible();
    if (shortestPath)
    {
        path = shortestPath;
        bool first = true;
        FVector prevPoint;
        UE_LOG(LogTemp, Warning, TEXT("test1: %d"), shortestPath->PathPoints.Num());
        for (FVector point : shortestPath->PathPoints)
        {
            DrawDebugSphere(GetWorld(), point, 10.0f, 4, FColor::Blue);
            if (!first) {
                DrawDebugLine(GetWorld(), prevPoint, point, FColor::Red, false);
            }
            first = false;
            prevPoint = point;
        }
    }
    //UE_LOG(LogTemp, Warning, TEXT("min: %f"), min);

}

UNavigationPath* ASDTAIController::FindClosestCollectible()
{
    TArray<AActor*> outCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), outCollectibles);
    UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

    UNavigationPath* shortestPath = nullptr;
    double min = DBL_MAX;
    for (AActor* collectible : outCollectibles)
    {
        UNavigationPath* collectiblePath = navSys->FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), collectible->GetActorLocation());

        double sum = 0;
        FVector prev;
        bool first = true;
        for (FVector point : collectiblePath->PathPoints)
        {
            if (!first) {
                sum += (point - prev).Size();
            }
            first = false;
            prev = point;
        }
        if (min > sum) {
            min = sum;
            shortestPath = collectiblePath;
        }
    }

    return shortestPath;
}

void ASDTAIController::OnMoveToTarget()
{
    m_ReachedTarget = false;
}

void ASDTAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    m_ReachedTarget = true;
}

void ASDTAIController::ShowNavigationPath()
{
    //Show current navigation path DrawDebugLine and DrawDebugSphere
}

void ASDTAIController::ChooseBehavior(float deltaTime)
{
    UpdatePlayerInteraction(deltaTime);
}

void ASDTAIController::UpdatePlayerInteraction(float deltaTime)
{
    //finish jump before updating AI state
    if (AtJumpSegment)
        return;

    APawn* selfPawn = GetPawn();
    if (!selfPawn)
        return;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
        return;

    FVector detectionStartLocation = selfPawn->GetActorLocation() + selfPawn->GetActorForwardVector() * m_DetectionCapsuleForwardStartingOffset;
    FVector detectionEndLocation = detectionStartLocation + selfPawn->GetActorForwardVector() * m_DetectionCapsuleHalfLength * 2;

    TArray<TEnumAsByte<EObjectTypeQuery>> detectionTraceObjectTypes;
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_COLLECTIBLE));
    detectionTraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(COLLISION_PLAYER));

    TArray<FHitResult> allDetectionHits;
    GetWorld()->SweepMultiByObjectType(allDetectionHits, detectionStartLocation, detectionEndLocation, FQuat::Identity, detectionTraceObjectTypes, FCollisionShape::MakeSphere(m_DetectionCapsuleRadius));

    FHitResult detectionHit;
    GetHightestPriorityDetectionHit(allDetectionHits, detectionHit);

    //Set behavior based on hit
    if (detectionHit.GetComponent()->GetCollisionObjectType() == COLLISION_PLAYER)
    {
        UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
        path = navSys->FindPathToLocationSynchronously(GetWorld(), GetPawn()->GetActorLocation(), detectionHit.GetActor()->GetActorLocation());
        bool first = true;
        FVector prevPoint;
        for (FVector point : path->PathPoints)
        {
            DrawDebugSphere(GetWorld(), point, 10.0f, 4, FColor::Blue);
            if (!first) {
                DrawDebugLine(GetWorld(), prevPoint, point, FColor::Red, false);
            }
            first = false;
            prevPoint = point;
        }
    }

    DrawDebugCapsule(GetWorld(), detectionStartLocation + m_DetectionCapsuleHalfLength * selfPawn->GetActorForwardVector(), m_DetectionCapsuleHalfLength, m_DetectionCapsuleRadius, selfPawn->GetActorQuat() * selfPawn->GetActorUpVector().ToOrientationQuat(), FColor::Blue);
}

void ASDTAIController::GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit)
{
    for (const FHitResult& hit : hits)
    {
        if (UPrimitiveComponent* component = hit.GetComponent())
        {
            if (component->GetCollisionObjectType() == COLLISION_PLAYER)
            {
                //we can't get more important than the player
                outDetectionHit = hit;
                return;
            }
            else if (component->GetCollisionObjectType() == COLLISION_COLLECTIBLE)
            {
                outDetectionHit = hit;
            }
        }
    }
}

void ASDTAIController::AIStateInterrupted()
{
    StopMovement();
    m_ReachedTarget = true;
}