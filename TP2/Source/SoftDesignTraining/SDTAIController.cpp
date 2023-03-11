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

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<USDTPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
}

FVector* FindTarget(UWorld* world, FVector location)
{
    TArray<AActor*> outCollectibles;
    UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), outCollectibles);

    FVector* closestLocation = nullptr;
    double min = DBL_MAX;

    for (AActor* actor : outCollectibles)
    {
        ASDTCollectible* collectible = Cast<ASDTCollectible>(actor);
        if (collectible->IsOnCooldown()) continue;

        FVector collectibleLocation = collectible->GetActorLocation();
        float dist = FVector::Distance(location, collectibleLocation);
        if (dist >= min) continue;
        
        closestLocation = &collectibleLocation;
        min = dist;
    }

    return closestLocation;
}

void ASDTAIController::GoToBestTarget(float deltaTime)
{
    //Move to target depending on current behavior

   // FVector* target = FindTarget(GetPawn()->GetWorld(), GetPawn()->GetActorLocation());
   //if (target == nullptr) return;

    //UE_LOG(LogTemp, Warning, TEXT("moveto %s"), *target->ToString());
    //this->MoveToLocation(*target);
    //OnMoveToTarget();

    UNavigationPath* path = FindClosestCollectible(GetWorld());
    FVector prev;
     for (FVector point : path->PathPoints)
     {
         MoveToLocation(point);
         prev = point;
         OnMoveToTarget();
        }
    

}

UNavigationPath* ASDTAIController::FindClosestCollectible(UWorld* world)
{
    TArray<AActor*> outCollectibles;
    UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), outCollectibles);
    UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(world);

    UNavigationPath* shortestPath = nullptr;
    double min = DBL_MAX;
    for (AActor* collectible : outCollectibles)
    {
        UNavigationPath* path = navSys->FindPathToLocationSynchronously(world, GetPawn()->GetActorLocation(), collectible->GetActorLocation());

        double sum = 0;
        FVector  prev;
        bool first = true;
        for (FVector point : path->PathPoints)
        {
            //if (!first) {
                sum += (point - prev).Size();
            //}
            //first = false;
            prev = point;
        }
        if (min > sum) {
            min = sum;
            closest = collectible;
            shortestPath = path;
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

    UWorld* world = GetPawn()->GetWorld();
    FNavPathSharedPtr path = GetPathFollowingComponent()->GetPath();
    if (!path) return;
    
    bool first = true;
    FNavPathPoint prevPoint;

    UE_LOG(LogTemp, Warning, TEXT("points"));
    for (FNavPathPoint& point : path->GetPathPoints())
    {
        UE_LOG(LogTemp, Warning, TEXT("  %s"), *point.Location.ToString());

        DrawDebugSphere(world, point.Location, 10.0f, 4, FColor::Blue);
        //if (!first) {
            DrawDebugLine(world, prevPoint.Location, point.Location, FColor::Red);
            //first = false;
        //}
        
        prevPoint = point;
    }
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