// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{

}

void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    Super::FollowPathSegment(DeltaTime);
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();
    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    // UE_LOG(LogTemp, Warning, TEXT("FollowPathSegment"));

    ASDTAIController* controller = dynamic_cast<ASDTAIController*>(GetOwner());
    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        //update jump
        controller->AtJumpSegment = true;
        GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("===========================")));
    }
    else
    {
        //update navigation along path
        
    }
}

void USDTPathFollowingComponent::SetMoveSegment(int32 segmentStartIndex)
{
    // UE_LOG(LogTemp, Warning, TEXT("SetMoveSegment"));
    Super::SetMoveSegment(segmentStartIndex);

    const TArray<FNavPathPoint>& points = Path->GetPathPoints();

    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];

    if (SDTUtils::HasJumpFlag(segmentStart) && FNavMeshNodeFlags(segmentStart.Flags).IsNavLink())
    {
        //Handle starting jump
        Cast<UCharacterMovementComponent>(MovementComp)->SetMovementMode(MOVE_Flying);
    }
    else
    {
        //Handle normal movement
    }
}

