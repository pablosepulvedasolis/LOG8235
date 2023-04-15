// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_MoveToPlayer.h"

#include "SoftDesignTraining.h"
#include "Kismet/KismetMathLibrary.h"
#include "SDTUtils.h"
#include "EngineUtils.h"

#include <SoftDesignTraining/SDTAIController.h>


EBTNodeResult::Type UMyBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		FVector playerLocation = GetPlayerLocation(aiController);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), playerLocation);
		//DrawDebugSphere(GetWorld(), playerLocation + FVector(0.f, 0.f, 100.f), 25.f,32, FColor::Red, false, NULL, NULL, NULL);
		aiController->MoveTo(playerLocation);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

FVector UMyBTTask_MoveToPlayer::GetPlayerLocation(ASDTAIController* aiController)
{
	ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (playerCharacter)
		return playerCharacter->GetActorLocation();
	return FVector();
}

