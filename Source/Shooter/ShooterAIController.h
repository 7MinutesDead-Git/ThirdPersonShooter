// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterAIController.generated.h"


// --------------------------------------------------------------
class UBlackboardComponent;
class UCharacterMovementComponent;

// --------------------------------------------------------------
UCLASS()
class SHOOTER_API AShooterAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UBlackboardComponent* BB;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;

	UPROPERTY()
	APawn* Player;
	UPROPERTY(EditAnywhere)
	float FollowDistance;
	UPROPERTY(EditAnywhere)
	float TimeToLoseInterest = 5;
	float Elapsed;
	float LastKnownPredictionLimit = 1;

	bool SeenPlayerBefore = false;

	FVector PatrolPosition;
	FVector LastKnownPosition;
};
