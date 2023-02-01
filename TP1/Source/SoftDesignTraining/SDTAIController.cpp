// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"

void ASDTAIController::Tick(float deltaTime)
{
    APawn* const pawn = GetPawn();

    DetectWall(pawn);
    Move(pawn, deltaTime);
}

void ASDTAIController::Move(APawn* const pawn, float deltaTime)
{
    //pawn->AddMovementInput(dir, speed);

    //UE_LOG(LogTemp, Warning, TEXT("dir: %s"), *dir.ToString());

    // Get new angle
    if (isTurning)
    {
        pawn->AddActorWorldRotation(FRotator(0, deltaAngle, 0));
        dir = dir.RotateAngleAxis(deltaAngle, FVector(0.0f, 0.0f, 1.0f));
        
        /*if (di)
        {

        }*/
    }

    // Get new speed
    speed += dir.GetSafeNormal(1) * acceleration * deltaTime;
    // Limit to max speed
    if (abs(speed.Size()) > maxSpeed) speed = maxSpeed * speed.GetSafeNormal(1);
    // Define delta movement
    FVector movement = speed * deltaTime;

    // Move actor
    FVector position = pawn->GetActorLocation() + movement * 100;
    pawn->SetActorLocation(position);
}

void ASDTAIController::DetectWall(APawn* const pawn)
{
    FVector position = pawn->GetActorLocation();
    bool seesWall = SDTUtils::Raycast(GetWorld(), position, position + dir * sightDistance * 100);
    if (seesWall) Turn(pawn);
}

void ASDTAIController::Turn(APawn* const pawn)
{
    isTurning = true;

    FVector position = pawn->GetActorLocation();

    FVector dir1(-dir.Y, dir.X, 0.0f);
    FVector dir2(dir.Y, -dir.X, 0.0f);

    bool seesWall1 = SDTUtils::Raycast(GetWorld(), position, position + dir1 * sightDistance * 100);
    bool seesWall2 = SDTUtils::Raycast(GetWorld(), position, position + dir2 * sightDistance * 100);

    if (seesWall1 && seesWall2) targetDir = -dir;
    else if (seesWall1 && !seesWall2) targetDir = dir2;
    else if (seesWall2 && !seesWall1) targetDir = dir1;
    else
    {
        targetDir = lastRandomDirWas1 ? dir2 : dir1;
        lastRandomDirWas1 = !lastRandomDirWas1;
    }

    // Turn "left" 
    isTurningPositive = IsTargetToTheLeft();
}

bool ASDTAIController::IsTargetToTheLeft()
{
    return dir.CosineAngle2D(targetDir - PI / 2) > 0;
}


