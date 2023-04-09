// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTService_TryDetectPlayer.h"

#include <SoftDesignTraining/SDTAIController.h>
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "DrawDebugHelpers.h"

UMyBTService_TryDetectPlayer::UMyBTService_TryDetectPlayer() {
	bCreateNodeInstance = true;
}

void UMyBTService_TryDetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

	if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
	{
		bool isPlayerDetected = aiController->TryDetectPlayer();
		DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), isPlayerDetected ? TEXT("Player detected") : TEXT("Player not detected"), aiController->GetPawn(), FColor::Blue, 0.4f, false);
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetIsPlayerDetectedKeyID(), isPlayerDetected);
	}
}