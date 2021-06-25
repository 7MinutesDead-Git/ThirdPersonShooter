// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBlackboardValue.h"

#include "BehaviorTree/BlackboardComponent.h"

// ----------------------------------------------------------------
UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue()
{
	NodeName = TEXT("Clear Blackboard Value");
}

// ----------------------------------------------------------------
// Ran once task is called.
// We will get the key selected in the behavior tree for this node, clear its value, then let caller know we succeeded.
EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	FName SelectedKey = GetSelectedBlackboardKey();
	OwnerComp.GetBlackboardComponent()->ClearValue(SelectedKey);
	return EBTNodeResult::Succeeded;
}
