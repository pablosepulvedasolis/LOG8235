// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoftDesignTraining/SoftDesignTrainingMainCharacter.h"

#include "SDTAIController.generated.h"

#define PI 3.141592f

/**
 *
 */
UCLASS( ClassGroup = AI, config = Game )
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick( float deltaTime ) override;

	void Move( APawn* const pawn, float deltaTime );
	void Turn( APawn* const pawn, float deltaTime );
	
	void HandleEntities( APawn* const pawn );
	void HandlePlayer( APawn* const pawn, ASoftDesignTrainingMainCharacter* const player );
	void PickUpDetection( APawn* const pawn );
	void EvadeWall( APawn* const pawn );
	void EvadeDeathFloor( APawn* const pawn );
	void StartEvading( APawn* const pawn );
	void SetTurning( APawn* const pawn, FVector targetDirection, bool overridable = false );
	
	void ChasePlayer( APawn* const pawn, AActor* player );
	void FleePlayer( APawn* const pawn, AActor* player );
	void PickUpDetectionSingle( APawn* const pawn, AActor* collectibleActor );
	bool DetectWall( APawn* const pawn, FVector3d* dir = nullptr );
	bool DetectDeathFloor( APawn* const pawn, FVector3d* dir = nullptr );

	virtual void BeginPlay() override;

	void DrawVisionSphere( APawn* const pawn );
	void DrawVisionCone( APawn* const pawn );
	
	ASoftDesignTrainingMainCharacter* GetPlayerToHandle( APawn* const pawn );
	FVector3d GetTargetDirectionFromOtherWalls( APawn* const pawn, FVector* currentDirection = nullptr );
	FVector GetNextTargetDir( FVector newDir, FHitResult wall );
	bool IsInsideSphere( APawn* const pawn, AActor* targetActor );
	bool IsInsideCone( APawn* const pawn, AActor* targetActor );

	void IncrementDeathCount();
	void IncrementPickUpCount();
	void DisplayTestResults( float deltaTime );

	/** Radisus of spehere detection of collectibles and player in cm. Value must be between 300 and 1000 cm  */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "300.0", ClampMax = "1000.0"))
		float detectionRadius = 800.0f;
	/** Angel of cone detection of collectibles and player in dgrees . Value must be between 30 and 90°  */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI, meta = (ClampMin = "10.0", ClampMax = "90.0"))
		float visionAngle = 50.0f ;
private:

	UPROPERTY( EditAnywhere )
		float acceleration = 20.0f; // m/s2
	UPROPERTY( EditAnywhere )
		float maxSpeed = 3.0f; // m/s
	UPROPERTY( EditAnywhere )
		float rotateSpeed = 500.0f;
	UPROPERTY( EditAnywhere )
		float sightDistance = 2.0f; // m
	UPROPERTY( EditAnywhere )
		float timeLength = 60; // s

	FVector speed = FVector( 0.0f, 0.0f, 0.0f ); // m/s
	FVector dir;
	FVector targetDir = dir;
	bool isTurningPositive = false;
	bool isTurning = false;
	bool canTurningBeOverriden = false;
	bool zigzagToggle = false;
	bool IsTargetToTheLeft();

	float timer = 0.f;
	int pickupCount = 0;
	int deathCount = 0;
};
