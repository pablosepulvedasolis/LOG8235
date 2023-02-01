// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

#define PI 3.141592f

/**
 *
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float deltaTime) override;
	void Move(APawn* const pawn, float deltaTime);
	void Turn(APawn* const pawn);
	void DetectWall(APawn* const pawn);
	FVector GetNextTargetDir(FVector newDir, FHitResult wall);
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		float acceleration = 20.0f; // m/s2
	UPROPERTY(EditAnywhere)
		float maxSpeed = 3.0f; // m/s
	UPROPERTY(EditAnywhere)
		float rotateSpeed = 500.0f;
	UPROPERTY(EditAnywhere)
		float sightDistance = 2.0f; // m

	FVector speed = FVector(0.0f, 0.0f, 0.0f); // m/s
	FVector dir;
	FVector targetDir = dir;
	bool isTurningPositive = false;
	bool isTurning = false;
	bool lastRandomDirWas1 = false;

	bool IsTargetToTheLeft();
};
