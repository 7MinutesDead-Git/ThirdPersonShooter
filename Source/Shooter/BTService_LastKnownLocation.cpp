// Behavior Tree service to update Player Location based on line of sight.
// Clears the value if we lose LOS.

#include "BTService_LastKnownLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

// ------------------------------------------------------------
UBTService_LastKnownLocation::UBTService_LastKnownLocation()
{
	NodeName = TEXT("Update Known Player Location if LOS");
}

// ------------------------------------------------------------
void UBTService_LastKnownLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Protection from null pointers.
	if (!Player || !OwnerComp.GetAIOwner()) return;

	// Being more verbose here so I can remember what's going on.
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	FName PlayerLocationKey = GetSelectedBlackboardKey();
	FVector PlayerLocation = Player->GetActorLocation();

	bool CanSeePlayer = OwnerComp.GetAIOwner()->LineOfSightTo(Player);

	if (CanSeePlayer) {
		BB->SetValueAsVector(PlayerLocationKey, PlayerLocation);
	}
	else {
		BB->ClearValue(PlayerLocationKey);
	}
}
