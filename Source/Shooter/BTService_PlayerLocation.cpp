// A BT Service to Get the player location and update the Key for it.
#include "BTService_PlayerLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

// --------------------------------------------------------------
UBTService_PlayerLocation::UBTService_PlayerLocation()
{
	NodeName = TEXT("Update Player Location");
}

// --------------------------------------------------------------
void UBTService_PlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Protection from null pointers.
	if (!Player || !OwnerComp.GetAIOwner()) return;

	// Being more verbose here so I can remember what's going on.
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	FName PlayerLocationKey = GetSelectedBlackboardKey();
	FVector PlayerLocation = Player->GetActorLocation();

	// Set our player location as the current key for this node in the Behavior Tree.
	BB->SetValueAsVector(PlayerLocationKey, PlayerLocation);
}
