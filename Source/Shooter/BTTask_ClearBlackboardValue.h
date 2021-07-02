// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardValue.generated.h"

/**
 *
 */
UCLASS()
class SHOOTER_API UBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    // Constructor.
    UBTTask_ClearBlackboardValue();

protected:
    /** Starts this task. Should return Succeeded, Failed or InProgress.
    *  (Use FinishLatentTask() when returning InProgress) \n\n
    * This function should be considered as const (don't modify state of object) if node is not instanced! */
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
