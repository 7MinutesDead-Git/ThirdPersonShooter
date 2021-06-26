// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AIController.h"
#include "ShooterCharacter.h"

// ----------------------------------------------------------------
UBTTask_Attack::UBTTask_Attack()
{
	// Give a name to this BT Node so we know what it is in the editor.
	NodeName = TEXT("Attack Task");
}

// ----------------------------------------------------------------
// Ran once task is called by the behavior tree.
EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// How to get the AI Controller from this Task, so we can fire off the attack function.
	AShooterCharacter* AIShooterCharacter = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	AIShooterCharacter->AttackBasic();
	return EBTNodeResult::Succeeded;
}
