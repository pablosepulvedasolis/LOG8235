// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDTBaseAIController.h"
#include "SDTAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
	GENERATED_BODY()

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleHalfLength = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleRadius = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleForwardStartingOffset = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    UCurveFloat* JumpCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpApexHeight = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpSpeed = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool AtJumpSegment = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool InAir = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool Landing = false;

    //my code 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
    double speed = 0.0;

    /** Radisus of spehere detection of collectibles and player in cm. Value must be between 300 and 1000 cm  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "300.0", ClampMax = "1000.0"))
        float detectionRadius = 800.0f;

    /** Angel of cone detection of collectibles and player in dgrees . Value must be between 30 and 90°  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "10.0", ClampMax = "90.0"))
        float visionAngle = 50.0f;

    enum State
    {
        Collect,
        Chase,
        Flee

    };
    State state = State::Collect;

    bool IsInsideSphere(APawn* const pawn, AActor* targetActor);
    bool IsInsideCone(APawn* const pawn, AActor* targetActor);

    void DrawVisionSphere(APawn* const pawn);
    void DrawVisionCone(APawn* const pawn);

    bool IsVisible(APawn* const pawn, AActor* player);

    UNavigationPath* FindFarthestFleeLocation(UWorld* world);

    


public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    void AIStateInterrupted();

protected:
    void OnMoveToTarget();
    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    void UpdatePlayerInteraction(float deltaTime);

private:
    AActor* closest;
    AActor* farthestFleeLocation;
    virtual void GoToBestTarget(float deltaTime) override;
    virtual void ChooseBehavior(float deltaTime) override;
    virtual void ShowNavigationPath() override;
    UNavigationPath* FindClosestCollectible(UWorld* world);
};
