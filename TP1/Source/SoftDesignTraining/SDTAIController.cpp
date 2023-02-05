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

void ASDTAIController::Tick( float deltaTime )
{
	APawn* pawn = GetPawn();
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), ASoftDesignTrainingMainCharacter::StaticClass(), foundActors );
	if ( pawn )
	{
		// TODO PickUp Collectible 

		if ( foundActors[0] )
		{


			EvadeWall( pawn );
			EvadeDeathFloor( pawn );
			Move( pawn, deltaTime );

			DisplayTestResults( deltaTime );

			DrawVisionSphere( GetWorld(), pawn, 26, FColor( 181, 0, 0 ) );
			FVector targetDirTemp = targetDir;

			if ( IsInsideSphere( pawn, foundActors[0] ) )
			{

				if ( !SDTUtils::IsPlayerPoweredUp( GetWorld() ) )
				{

					ChasePlayer( pawn, foundActors[0] );
				}
				/* else {

					 MoveToLocation(foundActors[0]->GetActorLocation() * FVector(-1.0f, -1.0f, 1.0f), 100.0f, true);
					 FVector direction = foundActors[0]->GetActorLocation() * FVector(-1.0f, -1.0f, 1.0f) - pawn->GetActorLocation();
					 pawn->SetActorRotation(direction.ToOrientationQuat());
				 }*/
			}
			else
			{
				PickUpDetection( pawn );
			}


		}

	}

}
void  ASDTAIController::DrawVisionSphere( UWorld* world, APawn* pawn, int32 segments, FColor color )
{
	DrawDebugSphere( world, pawn->GetActorLocation(), detectionRadius, segments, color );
}

bool ASDTAIController::IsInsideSphere( APawn* pawn, AActor* targetActor )
{

	if ( FVector::Dist2D( pawn->GetActorLocation(), targetActor->GetActorLocation() ) > detectionRadius )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void  ASDTAIController::DrawVisionCone( UWorld* world, APawn* pawn )
{
	DrawDebugCone( world, pawn->GetActorLocation(), pawn->GetActorForwardVector(), detectionRadius, visionAngle, visionAngle, 32, FColor::Green );
}
bool ASDTAIController::IsInsideCone( APawn* pawn, AActor* targetActor )
{

	auto pawnForwardVector = pawn->GetActorForwardVector();
	auto direction = targetActor->GetActorLocation() - pawn->GetActorLocation();

	auto value = FVector::DotProduct( direction.GetSafeNormal(), pawnForwardVector.GetSafeNormal() );
	auto angle = FMath::Acos( value );
	auto isVisible = FMath::Abs( angle ) <= visionAngle;
	return isVisible;
}

void ASDTAIController::PickUpDetection( APawn* pawn )
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), ASDTCollectible::StaticClass(), foundActors );

	for ( AActor* actor : foundActors ) PickUpDetectionSingle( pawn, actor );
}

void ASDTAIController::PickUpDetectionSingle( APawn* pawn, AActor* collectibleActor )
{
	ASDTCollectible* collectible = Cast<ASDTCollectible>( collectibleActor );

	bool isCollectibleInsideSphere = IsInsideSphere( pawn, collectible );
	if ( !isCollectibleInsideSphere ) return;

	bool isCollectibleVisible = collectible->GetStaticMeshComponent()->IsVisible();
	if ( !isCollectibleVisible ) return;

	DrawVisionCone( GetWorld(), pawn ); // for debbuging

	bool isCollectibleInCone = IsInsideCone( pawn, collectible );
	if ( !isCollectibleInCone ) return;

	DrawDebugSphere( GetWorld(), collectible->GetActorLocation(), 100.f, 32, FColor::Magenta ); //for debugging

	bool obstacleDetected = SDTUtils::Raycast( GetWorld(), pawn->GetActorLocation(), collectible->GetActorLocation() );
	if ( obstacleDetected ) return;

	isTurning = true;
	targetDir = FVector( FVector2D( collectible->GetActorLocation() - pawn->GetActorLocation() ), 0.0f ).GetSafeNormal();
	isTurningPositive = IsTargetToTheLeft();
}

void ASDTAIController::ChasePlayer( APawn* pawn, AActor* player )
{

	if ( IsInsideSphere( pawn, player ) )
	{

		bool obstacleDetected = SDTUtils::Raycast( GetWorld(), pawn->GetActorLocation(), player->GetActorLocation() );


		if ( !obstacleDetected )
		{
			isTurning = true;
			targetDir = FVector( FVector2D( player->GetActorLocation() - pawn->GetActorLocation() ), 0.0f ).GetSafeNormal();
			isTurningPositive = IsTargetToTheLeft();
		}


	}
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
		dir = targetDir;
		pawn->SetActorRotation( dir.Rotation() );
	}
}



void ASDTAIController::EvadeWall( APawn* const pawn )
{
	bool seesWall = DetectWall( pawn );
	if ( seesWall && !isTurning ) StartTurning( pawn );
}

void ASDTAIController::EvadeDeathFloor( APawn* const pawn )
{
	bool seesWall = DetectDeathFloor( pawn );
	if ( seesWall && !isTurning ) StartTurning( pawn );
}

void ASDTAIController::StartTurning( APawn* const pawn )
{
	isTurning = true;
	targetDir = GetTargetDirectionFromOtherWalls( pawn );
	isTurningPositive = IsTargetToTheLeft();
}



FVector3d ASDTAIController::GetTargetDirectionFromOtherWalls( APawn* const pawn )
{
	FVector position = pawn->GetActorLocation();

	FVector dir1( -dir.Y, dir.X, 0.0f );
	bool seesObstacle1 = DetectWall( pawn, &dir1 ) || DetectDeathFloor( pawn, &dir1 );

	FVector dir2( dir.Y, -dir.X, 0.0f );
	bool seesObstacle2 = DetectWall( pawn, &dir2 ) || DetectDeathFloor( pawn, &dir2 );

	FHitResult result = SDTUtils::RaycastInfo( GetWorld(), position, position + dir * sightDistance * 100 );

	if ( seesObstacle1 && seesObstacle2 ) return -dir;
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


