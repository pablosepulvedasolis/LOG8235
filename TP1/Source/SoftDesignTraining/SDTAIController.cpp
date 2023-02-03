// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SoftDesignTrainingCharacter.h"
#include "DrawDebugHelpers.h"

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();
    dir = GetPawn()->GetActorRotation().Vector();
}

void ASDTAIController::Tick(float deltaTime)
{
    APawn* const pawn = GetPawn();

    DetectWall(pawn);
    DetectDeathFloor(pawn);
    Move(pawn, deltaTime);
}

void ASDTAIController::Move(APawn* const pawn, float deltaTime)
{
    //pawn->AddMovementInput(dir, speed);

    //UE_LOG(LogTemp, Warning, TEXT("dir: %s"), *dir.ToString());

    // Get new angle
    if (isTurning)
    {
        float deltaAngle = (isTurningPositive ? 1 : -1) * deltaTime * rotateSpeed;

        dir = dir.RotateAngleAxis(deltaAngle, FVector(0.0f, 0.0f, 1.0f));
        pawn->SetActorRotation(dir.Rotation());

        float crossProduct = FVector3d::CrossProduct(dir, targetDir).Z;
        FVector position = pawn->GetActorLocation();

        if ((isTurningPositive ? 1 : -1) * crossProduct < 0)
        {
            isTurning = false;
            dir = targetDir;
            pawn->SetActorRotation(dir.Rotation());
        }

    }

    // Get new speed
    speed += dir.GetSafeNormal(0.0001) * acceleration * deltaTime;
    speed = speed.ProjectOnTo(dir);
    // Limit to max speed
    if (abs(speed.Size()) > maxSpeed) speed = maxSpeed * speed.GetSafeNormal(0.0001);
    // Define delta movement
    FVector movement = speed * deltaTime;

    // Move actor
    FVector position = pawn->GetActorLocation() + movement * 100;
    pawn->SetActorLocation(position);
}

void ASDTAIController::DetectWall(APawn* const pawn)
{
    FVector position = pawn->GetActorLocation();

    FVector rayVector = dir * sightDistance * 100;
    bool seesWall = SDTUtils::Raycast(GetWorld(), position, position + rayVector);
    if (seesWall && !isTurning) Turn(pawn);
}

void ASDTAIController::DetectDeathFloor(APawn* const pawn)
{
    FVector position = pawn->GetActorLocation();

    FVector rayVector = dir * sightDistance * 2 * 100;
    FVector3d axis = FVector::CrossProduct(dir, FVector::UnitZ());
    rayVector = rayVector.RotateAngleAxis(-22.5, axis);
    bool seesWall = SDTUtils::RaycastDeathFloor(GetWorld(), position, position + rayVector);
    if (seesWall && !isTurning) TurnDeathFloor(pawn);
}

void ASDTAIController::Turn(APawn* const pawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("Turn %d"), 0);
    isTurning = true;

    FVector position = pawn->GetActorLocation();

    FVector dir1(-dir.Y, dir.X, 0.0f);
    FVector dir2(dir.Y, -dir.X, 0.0f);

    bool seesWall1 = SDTUtils::Raycast(GetWorld(), position, position + dir1 * sightDistance * 100);
    bool seesWall2 = SDTUtils::Raycast(GetWorld(), position, position + dir2 * sightDistance * 100);

    FHitResult result = SDTUtils::RaycastInfo(GetWorld(), position, position + dir * sightDistance * 100);

    if (seesWall1 && seesWall2) targetDir = -dir;
    else if (seesWall1 && !seesWall2)
        targetDir = GetNextTargetDir(dir2, result);
    else if (seesWall2 && !seesWall1)
        targetDir = GetNextTargetDir(dir1, result);
    else
    {
        targetDir = lastRandomDirWas1 ? GetNextTargetDir(dir2, result) : GetNextTargetDir(dir1, result);
        lastRandomDirWas1 = !lastRandomDirWas1;
    }

    // Turn "left" 
    isTurningPositive = IsTargetToTheLeft();
}

void ASDTAIController::TurnDeathFloor(APawn* const pawn)
{
    // TODO (même implémentation que Turn() (projection à changer))
    isTurning = true;

    FVector position = pawn->GetActorLocation();

    FVector dir1(-dir.Y, dir.X, 0.0f);
    FVector dir2(dir.Y, -dir.X, 0.0f);

    bool seesWall1 = SDTUtils::Raycast(GetWorld(), position, position + dir1 * sightDistance * 100);
    bool seesWall2 = SDTUtils::Raycast(GetWorld(), position, position + dir2 * sightDistance * 100);

    FHitResult result = SDTUtils::RaycastInfo(GetWorld(), position, position + dir * sightDistance * 100);

    if (seesWall1 && seesWall2) targetDir = -dir;
    else if (seesWall1 && !seesWall2)
        targetDir = GetNextTargetDir(dir2, result);
    else if (seesWall2 && !seesWall1)
        targetDir = GetNextTargetDir(dir1, result);
    else
    {
        targetDir = lastRandomDirWas1 ? GetNextTargetDir(dir2, result) : GetNextTargetDir(dir1, result);
        lastRandomDirWas1 = !lastRandomDirWas1;
    }

    // Turn "left" 
    isTurningPositive = IsTargetToTheLeft();
}

FVector ASDTAIController::GetNextTargetDir(FVector newDir, FHitResult wall)
{
    FVector3d projected = FVector::VectorPlaneProject(newDir, wall.Normal);
    projected.Normalize();
    return projected;
}

bool ASDTAIController::IsTargetToTheLeft()
{
    return dir.CosineAngle2D(targetDir.RotateAngleAxis(PI / 2.0f, FVector3d(0.0f, 0.0f, 1.0f))) < 0;
}


