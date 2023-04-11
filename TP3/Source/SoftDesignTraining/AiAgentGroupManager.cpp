// Fill out your copyright notice in the Description page of Project Settings.


#include "AiAgentGroupManager.h"
#include "Engine/World.h"

AiAgentGroupManager* AiAgentGroupManager::m_Instance;

AiAgentGroupManager::AiAgentGroupManager()
{
}


AiAgentGroupManager* AiAgentGroupManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new AiAgentGroupManager();
    }

    return m_Instance;
}

void AiAgentGroupManager::Destroy()
{
    delete m_Instance;
    m_Instance = nullptr;
}

void AiAgentGroupManager::RegisterAIAgent(ASDTAIController* aiAgent)
{
    m_registeredAgents.AddUnique(aiAgent);
}

void AiAgentGroupManager::UnregisterAIAgent(ASDTAIController* aiAgent)
{
    m_registeredAgents.Remove(aiAgent);
}

void AiAgentGroupManager::DrawIndicatorSphere(ASDTAIController* aiAgent)
{
    DrawDebugSphere(aiAgent->GetPawn()->GetWorld(), aiAgent->GetPawn()->GetActorLocation() + FVector(0.f, 0.f, 100.f), 25.0f, 32, FColor::Purple);
    /*if (m_registeredAgents.Num() < 1) return;
    
    for (int i = 0; i < m_registeredAgents.Num(); i++) {

        if (m_registeredAgents[i]) {
            FVector agentPos = m_registeredAgents[i]->GetPawn()->GetActorLocation();
            DrawDebugSphere(m_registeredAgents[i]->GetWorld(), agentPos + FVector(0.f, 0.f, 100.f), 25.0f, 32, FColor::Purple);
        }*/
        
}

