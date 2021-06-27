// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGameMode.h"

void ASurvivalGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	UE_LOG(LogTemp, Warning, TEXT("%s died."), *PawnKilled->GetName());

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController) {
		// False means this is not the winning controller, because they just died.
		PlayerController->GameHasEnded(nullptr, false);
	}
}
