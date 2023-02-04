// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTraining/SoftDesignTrainingMainCharacter.h"
#include "SDTUtils.h"
#include "SoftDesignTraining/SDTCollectible.h"
#include "SoftDesignTrainingCharacter.h"

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();
    dir = GetPawn()->GetActorRotation().Vector();
}

void ASDTAIController::Tick(float deltaTime)
{
    APawn* pawn = GetPawn();
    TArray<AActor*> foundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoftDesignTrainingMainCharacter::StaticClass(), foundActors);
    if (pawn) {
        // TODO PickUp Collectible 

        if (foundActors[0]) {


            DetectWall(pawn);
            DetectDeathFloor(pawn);
            Move(pawn, deltaTime);

            DisplayTestResults(deltaTime);

            DrawVisionSphere(GetWorld(), pawn, 26, FColor(181, 0, 0));
            FVector targetDirTemp = targetDir;

            if (IsInsideSphere(pawn, foundActors[0])) {

                if (!SDTUtils::IsPlayerPoweredUp(GetWorld())) {

                    ChasePlayer(pawn, foundActors[0]);
                }
                /* else {

                     MoveToLocation(foundActors[0]->GetActorLocation() * FVector(-1.0f, -1.0f, 1.0f), 100.0f, true);
                     FVector direction = foundActors[0]->GetActorLocation() * FVector(-1.0f, -1.0f, 1.0f) - pawn->GetActorLocation();
                     pawn->SetActorRotation(direction.ToOrientationQuat());
                 }*/
            }
            else {
                PickUpDetection(pawn);
            }


        }

    }

}
void  ASDTAIController::DrawVisionSphere(UWorld* world, APawn* pawn, int32 segments, FColor color) {
    DrawDebugSphere(world, pawn->GetActorLocation(), detectionRadius, segments, color);
}

bool ASDTAIController::IsInsideSphere(APawn* pawn, AActor* targetActor) {

    if (FVector::Dist2D(pawn->GetActorLocation(), targetActor->GetActorLocation()) > detectionRadius)
    {
        return false;
    }
    else {
        return true;
    }
}

void  ASDTAIController::DrawVisionCone(UWorld* world, APawn* pawn) {
    DrawDebugCone(world, pawn->GetActorLocation(), pawn->GetActorForwardVector(), detectionRadius, visionAngle, visionAngle, 32, FColor::Green);
}
bool ASDTAIController::IsInsideCone(APawn* pawn, AActor* targetActor) {

    auto pawnForwardVector = pawn->GetActorForwardVector();
    auto direction = targetActor->GetActorLocation() - pawn->GetActorLocation();

    auto value = FVector::DotProduct(direction.GetSafeNormal(), pawnForwardVector.GetSafeNormal());
    auto angle = FMath::Acos(value);
    auto isVisible = FMath::Abs(angle) <= visionAngle;
    return isVisible;
}

void ASDTAIController::PickUpDetection(APawn* pawn) {
    TArray<AActor*> foundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundActors);
    for (int i = 0; i < foundActors.Num(); ++i)
    {

        ASDTCollectible* collectible = Cast<ASDTCollectible>(foundActors[i]);

        if (IsInsideSphere(pawn, foundActors[i]) && collectible->GetStaticMeshComponent()->IsVisible()) {

            DrawVisionCone(GetWorld(), pawn); // for debbuging

            if (IsInsideCone(pawn, foundActors[i])) {

                DrawDebugSphere(GetWorld(), foundActors[i]->GetActorLocation(), 100.f, 32, FColor::Magenta); //for debugging

                bool obstacleDetected = SDTUtils::Raycast(GetWorld(), pawn->GetActorLocation(), foundActors[i]->GetActorLocation());

                if (!obstacleDetected) {

                    isTurning = true;

                    targetDir = FVector(FVector2D(foundActors[i]->GetActorLocation() - pawn->GetActorLocation()), 0.0f).GetSafeNormal();
                    isTurningPositive = IsTargetToTheLeft();

                }

            }

        }

    }

}

void ASDTAIController::ChasePlayer(APawn* pawn, AActor* player) {

    if (IsInsideSphere(pawn, player)) {

        bool obstacleDetected = SDTUtils::Raycast(GetWorld(), pawn->GetActorLocation(), player->GetActorLocation());


        if (!obstacleDetected) {
            isTurning = true;
            targetDir = FVector(FVector2D(player->GetActorLocation() - pawn->GetActorLocation()), 0.0f).GetSafeNormal();
            isTurningPositive = IsTargetToTheLeft();
        }


    }
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

void ASDTAIController::IncrementDeathCount()
{
    deathCount++;
}

void ASDTAIController::IncrementPickUpCount()
{
    pickupCount++;
}

void ASDTAIController::DisplayTestResults(float deltaTime)
{
    timer += deltaTime;
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("===========================")));
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Compteur de temps : %s"), *FString::FromInt(timer)));
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Nombre de pickup ramassé : %s"), *FString::FromInt(pickupCount)));
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Nombre de mort de l'agent : %s"), *FString::FromInt(deathCount)));
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("==== %s ===="), *(GetPawn()->GetName())));

    if (timer >= timeLength)
    {
        timer = 0;
        deathCount = 0;
        pickupCount = 0;
    }
}


