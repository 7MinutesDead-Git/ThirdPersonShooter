// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"

#include "ShooterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// -------------------------------------
void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to player in world.
	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PatrolPosition = K2_GetActorLocation();

	// UBehaviorTree for AI!
	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);
		BB = GetBlackboardComponent();
		BB->SetValueAsVector(TEXT("AI Home Location"), PatrolPosition);
	}
}


// -------------------------------------
void AShooterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// -------------------------------------
/// Check IsDead from Character class.
bool AShooterAIController::IsDead() const
{
	AShooterCharacter* AICharacter = Cast<AShooterCharacter>(GetPawn());
	if (AICharacter) {
		return AICharacter->IsDead();
	}
	// If we don't have a pawn (detached), then surely this controller is dead.
	return true;
}
