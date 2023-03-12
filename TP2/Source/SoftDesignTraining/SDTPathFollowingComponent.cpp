// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTPathFollowingComponent.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SDTAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"
#include <Kismet/KismetMathLibrary.h>

USDTPathFollowingComponent::USDTPathFollowingComponent(const FObjectInitializer& ObjectInitializer)
{
    Initialize();
}

void USDTPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
    const TArray<FNavPathPoint>& points = Path->GetPathPoints();

    const FNavPathPoint& segmentStart = points[MoveSegmentStartIndex];
    FNavPathPoint segmentEnd;
    if (points.Num() > MoveSegmentEndIndex) {
        segmentEnd = points[MoveSegmentEndIndex];
    }

    FVector start = segmentStart.Location;
    FVector end = segmentEnd.Location;

    ASDTAIController* controller = dynamic_cast<ASDTAIController*>(GetOwner());
    if (SDTUtils::HasJumpFlag(segmentStart))
    {
        //update jump
        controller->AtJumpSegment = true;

        AActor* pawn = controller->GetPawn();
        UCurveFloat* jumpCurve = controller->JumpCurve;
        float jumpSpeed = controller->JumpSpeed;
        float jumpApexHeight = controller->JumpApexHeight;
        FVector vecToEnd = FVector(end - start);
        vecToEnd.Z = 0.f;

        float jumpTime = lastJumpTime + DeltaTime*jumpSpeed;
        lastJumpTime = jumpTime;

        FVector newPos = start + vecToEnd * jumpTime;
        float curveValue = jumpCurve->GetFloatValue(jumpTime);
        newPos.Z = jumpApexHeight * curveValue + 216.f;

        pawn->SetActorLocation(newPos, true);
        if (jumpTime >= 1) {
            pawn->SetActorLocation(end);
        }

        FVector pawnPos = pawn->GetActorLocation();
        FRotator pawnRot = UKismetMathLibrary::FindLookAtRotation(pawnPos, end);
        pawnRot.Pitch = 0;
        pawn->SetActorRotation(pawnRot);

        if (pawnPos.Z >= 260) {
            controller->InAir = true;
        }

        if (pawnPos.Z <= 265) {
            controller->Landing = true;
        }
    }
    else
    {
        //update navigation along path
        Super::FollowPathSegment(DeltaTime);
        controller->AtJumpSegment = false;
        controller->InAir = false;
        controller->Landing = false;
        lastJumpTime = 0;
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
        Cast<UCharacterMovementComponent>(MovementComp)->SetMovementMode(MOVE_Walking);
    }
}

