// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SDTCollectible.generated.h"

/**
 *
 */
UCLASS()
class SOFTDESIGNTRAINING_API ASDTCollectible : public AStaticMeshActor
{
    GENERATED_BODY()
public:
    ASDTCollectible();

    void Collect();
    void OnCooldownDone();
    bool IsOnCooldown();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        float m_CollectCooldownDuration = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool isMoveable = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        FVector acceleration = FVector(0.0f, 250.0f, 0.0f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        FVector speed = FVector(0.0f, 0.0f, 0.0f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        FVector maxSpeed = FVector(0.0f, 500.0f, 0.0f);
    //debbug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool rightWallDetected;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool leftWallDetected;

    virtual void Tick(float deltaTime) override;
    virtual void BeginPlay() override;

    FVector initialPosition;

protected:
    FTimerHandle m_CollectCooldownTimer;


};
