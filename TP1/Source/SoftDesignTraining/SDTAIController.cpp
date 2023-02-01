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
        UE_LOG(LogTemp, Warning, TEXT("deltaAngle: %f"), deltaAngle);

        dir = dir.RotateAngleAxis(deltaAngle, FVector(0.0f, 0.0f, 1.0f));
        pawn->SetActorRotation(dir.Rotation());

        float crossProduct = FVector3d::CrossProduct(dir, targetDir).Z;
        FVector position = pawn->GetActorLocation();
        DrawDebugLine(GetWorld(), position, position + FVector3d::CrossProduct(dir, targetDir) * sightDistance * 100, FColor::White);
        UE_LOG(LogTemp, Warning, TEXT("crossProduct: %f"), crossProduct);

        if ((isTurningPositive ? 1 : -1) * crossProduct < 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("DONE TURNING %f"), 0);
            isTurning = false;
            dir = targetDir;
            FVector3d newRotation = FVector3d::CrossProduct(FVector(0, 1.0f, 0), dir);
            //pawn->SetActorRotation(FRotator(0, newRotation.Z, 0));
            UE_LOG(LogTemp, Warning, TEXT("dir: %s"), *dir.ToString());
            UE_LOG(LogTemp, Warning, TEXT("dir norm: %s"), *(dir.GetSafeNormal(1)).ToString());
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

    DrawDebugLine(GetWorld(), position, position + dir * sightDistance * 100, FColor::Red);
    bool seesWall = SDTUtils::Raycast(GetWorld(), position, position + dir * sightDistance * 100);
    if (seesWall && !isTurning) Turn(pawn);
}

void ASDTAIController::Turn(APawn* const pawn)
{
    UE_LOG(LogTemp, Warning, TEXT("TURNING %d"), 0);
    isTurning = true;

    FVector position = pawn->GetActorLocation();

    FVector dir1(-dir.Y, dir.X, 0.0f);
    FVector dir2(dir.Y, -dir.X, 0.0f);

    bool seesWall1 = SDTUtils::Raycast(GetWorld(), position, position + dir1 * sightDistance * 100);
    bool seesWall2 = SDTUtils::Raycast(GetWorld(), position, position + dir2 * sightDistance * 100);

    if (seesWall1)
    {
        DrawDebugLine(GetWorld(), position, position + dir1 * sightDistance * 100, FColor::Orange, false, 1.0f);
    }
    if (seesWall2)
    {
        DrawDebugLine(GetWorld(), position, position + dir2 * sightDistance * 100, FColor::Orange, false, 1.0f);
    }

    DrawDebugLine(GetWorld(), position, position + dir1 * sightDistance * 100, FColor::Blue);
    DrawDebugLine(GetWorld(), position, position + dir2 * sightDistance * 100, FColor::Green);

    if (seesWall1 && seesWall2) targetDir = -dir;
    else if (seesWall1 && !seesWall2) targetDir = dir2;
    else if (seesWall2 && !seesWall1) targetDir = dir1;
    else
    {
        targetDir = lastRandomDirWas1 ? dir2 : dir1;
        lastRandomDirWas1 = !lastRandomDirWas1;
    }

    DrawDebugLine(GetWorld(), position, position + targetDir * sightDistance * 100, FColor::Purple, false, 1.0f);

    // Turn "left" 
    isTurningPositive = IsTargetToTheLeft();
}

bool ASDTAIController::IsTargetToTheLeft()
{
    return dir.CosineAngle2D(targetDir.RotateAngleAxis(PI / 2.0f, FVector3d(0.0f, 0.0f, 1.0f))) < 0;
}


