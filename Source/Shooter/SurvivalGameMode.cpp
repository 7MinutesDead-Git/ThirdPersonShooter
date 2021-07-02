// * bool IsWinner covers all four scenarios.
//    If this is the player (true), and the player is the winner (true), then this controller IsWinner (true).
//    If this isn't the player (false), and the player isn't the winner (false), then this controller (AI) IsWinner (true).
//    If this isn't the player (false), and the player is the winner (true), then this controller (AI) is not the winner (false).
//    If this is our player (true), but the player isn't the winner (false), then this controller (player) is not the winner (false).


#include "SurvivalGameMode.h"

#include "EngineUtils.h"
#include "ShooterAIController.h"


// -------------------------------------
void ASurvivalGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    UE_LOG(LogTemp, Warning, TEXT("%s died."), *PawnKilled->GetName());

    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
    if (PlayerController) {
        // False means this is not the winning controller, because they just died.
        EndGame(false);
    }

    // Each time an NPC is killed, loop through all pawns to see if they're all dead yet.
    for (auto Controller : TActorRange<AShooterAIController>(GetWorld())) {
        if (!Controller->IsDead())
            return;
    }
    // If all AI above are dead, then we make it here.
    EndGame(true);
}

// -------------------------------------
void ASurvivalGameMode::EndGame(bool PlayerIsWinner)
{
    // Slow down time when game ends.
    GetWorldSettings()->SetTimeDilation(0.1);

    // Getting a list/array of Controller actors to iterate over, so we can let all players/NPCs know who won.
    for (auto Controller : TActorRange<AController>(GetWorld())) {
        // See notes at top.*
        bool IsWinner = Controller->IsPlayerController() == PlayerIsWinner;
        // TODO: Focus camera on this pawn from an orbiting view.
        Controller->GameHasEnded(
