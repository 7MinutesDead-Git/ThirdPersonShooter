#include "ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"

// -------------------------------------
void AShooterPlayerController::BeginPlay()
{
	HUD = CreateWidget(this, PlayerHUD);
	if (HUD) {
		HUD->AddToViewport();
	}
}

// -------------------------------------
void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	// Player wins.
	if (bIsWinner) {
		// Create Win summary screen attached to this PlayerController as owner.
		UUserWidget* SummaryScreen = CreateWidget(this, WinSummaryUI);
		if (SummaryScreen) {
			HUD->RemoveFromViewport();
			SummaryScreen->AddToViewport();
		}
	}
	// Player dies.
	else {
		// Restart level after short delay.
		GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);

		UUserWidget* DeathScreen = CreateWidget(this, DeathScreenUI);
		if (DeathScreen) {
			HUD->RemoveFromViewport();
			DeathScreen->AddToViewport();
		}
	}


}
