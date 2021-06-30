// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

// ---------------------
// Forward declarations.
class UUserWidget;

// ---------------------
UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void GameHasEnded(AActor* EndGameFocus, bool bIsWinner) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> DeathScreenUI;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WinSummaryUI;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PlayerHUD;
	UPROPERTY()
	UUserWidget* HUD;
	UPROPERTY(EditAnywhere)
	float RestartDelay = 2;
	FTimerHandle RestartTimer;
};
