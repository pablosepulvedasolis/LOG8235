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

    /** Acceleration of the collectible in cm/s2. Value most be between 100 and 1000 cm/s2 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "100.0", ClampMax = "1000.0"))
        float acceleration = 250.0f;
    /** Maximum speed of the collectible in cm/s. Value most be between 100 and 1000 cm/s */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "100.0", ClampMax = "1000.0"))
        float maxSpeed = 500.0f;
    /** Wall detection distance in cm. Value most be between 500 and 1000 cm */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI,meta = (ClampMin = "500.0" , ClampMax = "1000.0"))
        float wallDetectionDistance = 700.0f;

 

    virtual void Tick(float deltaTime) override;
    virtual void BeginPlay() override;

    FVector initialPosition;

protected:
    FTimerHandle m_CollectCooldownTimer;
    FVector speed = FVector(0.0f, 0.0f, 0.0f);


};
