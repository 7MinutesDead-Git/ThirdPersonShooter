// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// --------------------------------------------------------------
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


// --------------------------------------------------------------
void AShooterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool SeesPlayer = LineOfSightTo(Player);

	if (SeesPlayer) {
		BB->SetValueAsVector(TEXT("Player Location"), Player->GetActorLocation());
		BB->SetValueAsVector(TEXT("Last Known Player Location"), Player->GetActorLocation());
	}
	else {
		BB->ClearValue(TEXT("Player Location"));
	}
}
