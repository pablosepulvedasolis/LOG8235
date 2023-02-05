// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

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

	void EvadeWall( APawn* const pawn );
	void EvadeDeathFloor( APawn* const pawn );
	void StartTurning( APawn* const pawn );
	
	void PickUpDetection( APawn* pawn );
	void PickUpDetectionSingle( APawn* pawn, AActor* collectibleActor );
	bool DetectWall( APawn* const pawn, FVector3d* dir = nullptr );
	bool DetectDeathFloor( APawn* const pawn, FVector3d* dir = nullptr );

	virtual void BeginPlay() override;

	void DrawVisionSphere( UWorld* world, APawn* pawn, int32 segments, FColor color );
	void DrawVisionCone( UWorld* world, APawn* pawn );
	
	FVector3d GetTargetDirectionFromOtherWalls( APawn* const pawn );
	FVector GetNextTargetDir( FVector newDir, FHitResult wall );
	bool IsInsideSphere( APawn* pawn, AActor* targetActor );
	bool IsInsideCone( APawn* pawn, AActor* targetActor );
	void ChasePlayer( APawn* pawn, AActor* player );

	void IncrementDeathCount();
	void IncrementPickUpCount();
	void DisplayTestResults( float deltaTime );

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float detectionRadius = 400.0f;
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AI )
		float visionAngle = 50.0f * ( PI / 180.0f );
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
	bool zigzagToggle = false;
	bool IsTargetToTheLeft();

	float timer = 0.f;
	int pickupCount = 0;
	int deathCount = 0;
};
