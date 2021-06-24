// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"

#include "Kismet/GameplayStatics.h"


// --------------------------------------------------------------
void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to player in world.
	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PatrolPosition = K2_GetActorLocation();
	MoveToLocation(PatrolPosition);
}


// --------------------------------------------------------------
void AShooterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	bool SeesPlayer = LineOfSightTo(Player, FVector::ZeroVector, true);

	if (SeesPlayer) {
		SeenPlayer = true;
		Elapsed = 0;
		// Have this AI focus on the player.
		SetFocus(Player);
		// Move towards the player.
		MoveToActor(Player, FollowDistance);
		// Keep this up-to-date so we can move towards this once we lose LOS.
		LastKnownPosition = Player->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("Moving towards player."));
	}
	else if (SeenPlayer) {
		Elapsed += DeltaSeconds;

		// Follow beyond losing LOS so we appear to investigate a bit beyond the last seen corner.
		if (Elapsed <= LastKnownPredictionLimit) {
			UE_LOG(LogTemp, Warning, TEXT("Moving to last known location."));
			LastKnownPosition = Player->GetActorLocation();
		}

		// Constantly move towards last known position until we lose interest.
		MoveToLocation(LastKnownPosition);

		if (Elapsed > TimeToLoseInterest) {
			UE_LOG(LogTemp, Warning, TEXT("Heading home."));
			ClearFocus(EAIFocusPriority::Gameplay);
			MoveToLocation(PatrolPosition);
		}
	}
}
