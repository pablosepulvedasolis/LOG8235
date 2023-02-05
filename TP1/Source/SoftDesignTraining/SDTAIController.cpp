// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"
#include "SDTUtils.h"
#include "SoftDesignTraining/SDTCollectible.h"
#include "SoftDesignTrainingCharacter.h"

void ASDTAIController::BeginPlay()
{
	Super::BeginPlay();
	dir = GetPawn()->GetActorRotation().Vector();
}

void ASDTAIController::Tick( float deltaTime )
{
	APawn* const pawn = GetPawn();
	if ( pawn == nullptr ) return;
	
	HandleEntities( pawn );
	EvadeWall( pawn );
	EvadeDeathFloor( pawn );

	Move( pawn, deltaTime );

	DisplayTestResults( deltaTime );

	DrawVisionSphere( pawn );
	DrawVisionCone( pawn );
}

void ASDTAIController::HandleEntities( APawn* const pawn )
{
	ASoftDesignTrainingMainCharacter* player = GetPlayerToHandle( pawn );
	if ( player != nullptr ) HandlePlayer( pawn, player );
	else PickUpDetection( pawn );
}

ASoftDesignTrainingMainCharacter* ASDTAIController::GetPlayerToHandle( APawn* const pawn )
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), ASoftDesignTrainingMainCharacter::StaticClass(), foundActors );

	if ( foundActors.Num() == 0 ) return nullptr;
	if ( !IsInsideSphere( pawn, foundActors[0] ) ) return nullptr;

	return Cast<ASoftDesignTrainingMainCharacter>( foundActors[0] );
}

void ASDTAIController::HandlePlayer( APawn* const pawn, ASoftDesignTrainingMainCharacter* const player )
{
	bool isPlayerPoweredUp = SDTUtils::IsPlayerPoweredUp( GetWorld() );

	if ( !isPlayerPoweredUp ) ChasePlayer( pawn, player );
	else FleePlayer( pawn, player );
}

void ASDTAIController::PickUpDetection( APawn* const pawn )
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), ASDTCollectible::StaticClass(), foundActors );

	for ( AActor* actor : foundActors ) PickUpDetectionSingle( pawn, actor );
}

void ASDTAIController::PickUpDetectionSingle( APawn* const pawn, AActor* collectibleActor )
{
	ASDTCollectible* collectible = Cast<ASDTCollectible>( collectibleActor );

	bool isCollectibleInsideSphere = IsInsideSphere( pawn, collectible );
	if ( !isCollectibleInsideSphere ) return;

	bool isCollectibleVisible = collectible->GetStaticMeshComponent()->IsVisible();
	if ( !isCollectibleVisible ) return;

	bool isCollectibleInCone = IsInsideCone( pawn, collectible );
	if ( !isCollectibleInCone ) return;

	bool obstacleDetected = SDTUtils::Raycast( GetWorld(), pawn->GetActorLocation(), collectible->GetActorLocation() );
	if ( obstacleDetected ) return;

	FVector targetDirection = FVector( FVector2D( collectible->GetActorLocation() - pawn->GetActorLocation() ), 0.0f ).GetSafeNormal();
	SetTurning( pawn, targetDirection, true );
}

void ASDTAIController::ChasePlayer( APawn* const pawn, AActor* player )
{
	bool isPlayerInsideSphere = IsInsideSphere( pawn, player );
	if ( !isPlayerInsideSphere ) return;

	bool obstacleDetected = SDTUtils::Raycast( GetWorld(), pawn->GetActorLocation(), player->GetActorLocation() );
	if ( obstacleDetected ) return;

	FVector targetDirection = FVector( FVector2D( player->GetActorLocation() - pawn->GetActorLocation() ), 0.0f ).GetSafeNormal();

	// If the AI chases but there is a wall in front of him, take it into account
	bool hasObstacle = DetectWall( pawn, &targetDirection ) || DetectDeathFloor( pawn, &targetDirection );
	if ( hasObstacle ) targetDirection = GetTargetDirectionFromOtherWalls( pawn, &targetDirection );
	
	SetTurning( pawn, targetDirection, true );
}

void ASDTAIController::FleePlayer( APawn* const pawn, AActor* player )
{
	bool isPlayerInsideSphere = IsInsideSphere( pawn, player );
	if ( !isPlayerInsideSphere ) return;

	bool obstacleDetected = SDTUtils::Raycast( GetWorld(), pawn->GetActorLocation(), player->GetActorLocation() );
	if ( obstacleDetected ) return;

	FVector targetDirection = FVector( FVector2D( pawn->GetActorLocation() - player->GetActorLocation() ), 0.0f ).GetSafeNormal();

	// If the AI flees but there is a wall in front of him, take it into account
	bool hasObstacle = DetectWall( pawn, &targetDirection ) || DetectDeathFloor( pawn, &targetDirection );
	if ( hasObstacle ) targetDirection = GetTargetDirectionFromOtherWalls( pawn, &targetDirection );

	SetTurning( pawn, targetDirection, true );
}

void ASDTAIController::Move( APawn* const pawn, float deltaTime )
{
	if ( isTurning ) Turn( pawn, deltaTime );

	// Get new speed
	speed += dir.GetSafeNormal( 0.0001 ) * acceleration * deltaTime;
	speed = speed.ProjectOnTo( dir );
	// Limit to max speed
	if ( abs( speed.Size() ) > maxSpeed ) speed = maxSpeed * speed.GetSafeNormal( 0.0001 );

	// Move actor
	FVector position = pawn->GetActorLocation() + speed * deltaTime * 100;
	pawn->SetActorLocation( position );
}

void ASDTAIController::Turn( APawn* const pawn, float deltaTime )
{
	float deltaAngle = ( isTurningPositive ? 1 : -1 ) * deltaTime * rotateSpeed;

	dir = dir.RotateAngleAxis( deltaAngle, FVector( 0.0f, 0.0f, 1.0f ) );
	pawn->SetActorRotation( dir.Rotation() );

	float crossProduct = FVector3d::CrossProduct( dir, targetDir ).Z;
	FVector position = pawn->GetActorLocation();

	if ( ( isTurningPositive ? 1 : -1 ) * crossProduct < 0 )
	{
		isTurning = false;
		canTurningBeOverriden = false;
		dir = targetDir;
		pawn->SetActorRotation( dir.Rotation() );
	}
}



void ASDTAIController::EvadeWall( APawn* const pawn )
{
	bool seesWall = DetectWall( pawn );
	if ( seesWall && (!isTurning || canTurningBeOverriden) ) StartEvading(pawn);
}

void ASDTAIController::EvadeDeathFloor( APawn* const pawn )
{
	bool seesWall = DetectDeathFloor( pawn );
	if ( seesWall && (!isTurning || canTurningBeOverriden) ) StartEvading( pawn );
}

void ASDTAIController::StartEvading( APawn* const pawn )
{
	FVector targetDirection = GetTargetDirectionFromOtherWalls( pawn );
	SetTurning( pawn, targetDirection );
}

void ASDTAIController::SetTurning( APawn* const pawn, FVector targetDirection, bool overridable )
{
	isTurning = true;
	targetDir = targetDirection;
	isTurningPositive = IsTargetToTheLeft();
	canTurningBeOverriden = overridable;
}

void  ASDTAIController::DrawVisionSphere( APawn* const pawn )
{
	DrawDebugSphere( GetWorld(), pawn->GetActorLocation(), detectionRadius, 24, FColor::Silver);
}

bool ASDTAIController::IsInsideSphere( APawn* const pawn, AActor* targetActor )
{
	float distance = FVector::Dist2D( pawn->GetActorLocation(), targetActor->GetActorLocation() );
	return distance <= detectionRadius;
}

void  ASDTAIController::DrawVisionCone( APawn* const pawn )
{
	DrawDebugCone( GetWorld(), pawn->GetActorLocation(), pawn->GetActorForwardVector(), detectionRadius, visionAngle * (PI / 180.0f), visionAngle * (PI / 180.0f), 32, FColor::Green);
}
bool ASDTAIController::IsInsideCone( APawn* const pawn, AActor* targetActor )
{

	auto pawnForwardVector = pawn->GetActorForwardVector();
	auto direction = targetActor->GetActorLocation() - pawn->GetActorLocation();

	auto value = FVector::DotProduct( direction.GetSafeNormal(), pawnForwardVector.GetSafeNormal() );
	auto angle = FMath::Acos( value );
	auto isVisible = FMath::Abs( angle ) <= visionAngle * (PI / 180.0f);
	return isVisible;
}



FVector3d ASDTAIController::GetTargetDirectionFromOtherWalls( APawn* const pawn, FVector* currentDirection )
{
	if ( currentDirection == nullptr ) currentDirection = &dir;

	FVector position = pawn->GetActorLocation();

	FVector dir1( -currentDirection->Y, currentDirection->X, 0.0f );
	bool seesObstacle1 = DetectWall( pawn, &dir1 ) || DetectDeathFloor( pawn, &dir1 );

	FVector dir2( currentDirection->Y, -currentDirection->X, 0.0f );
	bool seesObstacle2 = DetectWall( pawn, &dir2 ) || DetectDeathFloor( pawn, &dir2 );

	FHitResult result = SDTUtils::RaycastInfo( GetWorld(), position, position + *currentDirection * sightDistance * 100 );

	if ( seesObstacle1 && seesObstacle2 ) return -*currentDirection;
	if ( seesObstacle1 && !seesObstacle2 ) return GetNextTargetDir( dir2, result );
	if ( !seesObstacle1 && seesObstacle2 ) return GetNextTargetDir( dir1, result );

	zigzagToggle = !zigzagToggle;
	return zigzagToggle ? GetNextTargetDir( dir1, result ) : GetNextTargetDir( dir2, result );
}


bool ASDTAIController::DetectWall( APawn* const pawn, FVector3d* direction )
{
	if ( direction == nullptr ) direction = &this->dir;

	FVector position = pawn->GetActorLocation();

	FVector rayVector = *direction * sightDistance * 100;
	return SDTUtils::Raycast( GetWorld(), position, position + rayVector );
}

bool ASDTAIController::DetectDeathFloor( APawn* const pawn, FVector3d* direction )
{
	if ( direction == nullptr ) direction = &this->dir;

	FVector position = pawn->GetActorLocation();

	FVector rayVector = *direction * sightDistance * 2 * 100;
	FVector3d axis = FVector::CrossProduct( *direction, FVector::UnitZ() );
	rayVector = rayVector.RotateAngleAxis( -22.5, axis );
	return SDTUtils::RaycastDeathFloor( GetWorld(), position, position + rayVector );
}

FVector ASDTAIController::GetNextTargetDir( FVector newDir, FHitResult wall )
{
	FVector3d projected = FVector::VectorPlaneProject( newDir, wall.Normal );
	projected.Normalize();
	return projected;
}

bool ASDTAIController::IsTargetToTheLeft()
{
	return dir.CosineAngle2D( targetDir.RotateAngleAxis( PI / 2.0f, FVector3d( 0.0f, 0.0f, 1.0f ) ) ) < 0;
}

void ASDTAIController::IncrementDeathCount()
{
	deathCount++;
}

void ASDTAIController::IncrementPickUpCount()
{
	pickupCount++;
}

void ASDTAIController::DisplayTestResults( float deltaTime )
{
	timer += deltaTime;
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Red, FString::Printf( TEXT( "===========================" ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Green, FString::Printf( TEXT( "Compteur de temps : %s" ), *FString::FromInt( timer ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Green, FString::Printf( TEXT( "Nombre de pickup ramassé : %s" ), *FString::FromInt( pickupCount ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Green, FString::Printf( TEXT( "Nombre de mort de l'agent : %s" ), *FString::FromInt( deathCount ) ) );
	GEngine->AddOnScreenDebugMessage( -1, 0, FColor::Red, FString::Printf( TEXT( "==== %s ====" ), *( GetPawn()->GetName() ) ) );

	if ( timer >= timeLength )
	{
		timer = 0;
		deathCount = 0;
		pickupCount = 0;
	}
}


