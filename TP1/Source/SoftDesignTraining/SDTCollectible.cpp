// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"
#include <SoftDesignTraining/SDTUtils.h>

ASDTCollectible::ASDTCollectible()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASDTCollectible::BeginPlay()
{
    Super::BeginPlay();
}

void ASDTCollectible::Collect()
{
    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);
}

void ASDTCollectible::OnCooldownDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_CollectCooldownTimer);

    GetStaticMeshComponent()->SetVisibility(true);
}

bool ASDTCollectible::IsOnCooldown()
{
    return m_CollectCooldownTimer.IsValid();
}

void ASDTCollectible::Tick(float deltaTime)
{
    Super::Tick(deltaTime);


    if (isMoveable == true) {
        bool rightWallDetected = SDTUtils::Raycast(GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0.0f, wallDetectionDistance, 0.0f));
        bool leftWallDetected = SDTUtils::Raycast(GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0.0f, -wallDetectionDistance, 0.0f));

        if (rightWallDetected) {
           acceleration = -abs(acceleration);
        }
        else if (leftWallDetected) {
           acceleration = abs(acceleration);
        }

        speed += FVector(0.0f,acceleration,0.0f) * deltaTime;
        if (abs(speed.Size()) > maxSpeed) speed = maxSpeed * speed.GetSafeNormal(); // put max speed in the right direction
        FVector delataX = (speed * deltaTime);
        SetActorLocation(GetActorLocation() + delataX, true);
    }


}
