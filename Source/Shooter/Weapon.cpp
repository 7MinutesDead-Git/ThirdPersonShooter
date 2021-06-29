// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Field/FieldSystemNodes.h"
#include "Kismet/GameplayStatics.h"


// -------------------------------------
/// Sets default values for AWeapon class. -7MD
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	DragonSwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dragon Sword Skeletal Mesh"));
	DragonSwordMesh->SetupAttachment(Root);
}

// -------------------------------------
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	RicochetBounceDelay = RicochetBounceStartDelay;
}

// -------------------------------------
// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// -------------------------------------
/// Perform the basic attack with this weapon. -7MD
void AWeapon::AttackBasic()
{
	RicochetBounceDelay = RicochetBounceStartDelay;
	RicochetBounces = 0;

	// Attack Sound.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FinalHitExplosionSound, GetActorLocation(), FRotator::ZeroRotator);

	// Attack particle effect.
	UGameplayStatics::SpawnEmitterAttached(
		WeaponFlash,		        // Emitter.
		DragonSwordMesh,	        // Component to attach to.
		TEXT("WeaponFlashSocket"),  // Bone/Socket to attach to.
		FVector::ZeroVector,		// Relative position.
		FRotator::ZeroRotator,		// Relative rotation.
		FVector(0.2, 0.2, 0.2)		// Scale.
		);

	// We want to shoot to where our view/camera is aiming.
	// To get our player view point from here, we need to access this weapon's owner's controller.
	// First we get the pawn.
	OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	// Then from the pawn we can get the controller.
	OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;
	// Then from the controller we can get the viewpoint.
	OwnerController->GetPlayerViewPoint(OUT StartLocation, OUT StartRotation);

	if (bToggleSelfRicochet) {
		// TODO: Disable gravity on player while bouncing around.
		// TODO: Add impulse for extra disorientation.
	}

	// Direction that points from the rotation.
	// We'll make this our first direction.
	RicochetDirection = StartRotation.Vector();

	// Start the ricochets.
	BounceImpact(StartLocation, RicochetDirection);
}

// -------------------------------------
/// Line trace to hit target.\n
/// Ricochet emitter rotation across hit surface.\n
///	Update StartDirection and RicochetDirection. \n
///	Recursively call itself again via TimerHandle to do next bounce.
void AWeapon::BounceImpact(FVector Start, FVector Direction)
{
	FHitResult Hit;
	bool HitSuccess = BounceLineTrace(Start, Direction, OUT Hit);

	if (HitSuccess) {
		// If we didn't hit a null actor, then deal damage to it.
		DoDamage(Hit, Direction);
		// Update ricochet info for additional BounceImpact() calls.
		ReflectForNextBounce(Hit, Direction);

		if (OwnerPawn && bToggleSelfRicochet) {
			// Allows us to bounce into the walls, hopefully without going inside them.
			// This will take us to the hit location, but stick us outward from the normal to avoid clipping.
			// A sweep would seem the better solution, but in practice it prevents complete movement.
			// TODO: A more elegant solution needs to be made. Leaving magic number here for now.
			FVector AdjustedDirection = Hit.Location + Hit.ImpactNormal.Rotation().Vector() * 300;
			OwnerPawn->SetActorLocation(AdjustedDirection);
		}

		// Spawn particles!
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), WeaponFlash, Hit.ImpactPoint, RicochetRotation, ImpactEffectScale
			);

		// Play sound!
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(), HitImpactSound, Hit.ImpactPoint, FRotator::ZeroRotator
			);

		// TODO: Have effect/emitter line travel from hand to impact point, then via bounces.
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Purple, false, 5);

		// Finally, do the next bounce.
		DoNextBounceImpact();
	}
}

// -------------------------------------
/// Deal damage to the actor hit, if actor is not null. -7MD
void AWeapon::DoDamage(FHitResult Hit, FVector Direction)
{
	AActor* HitActor = Hit.GetActor();

	if (HitActor) {

		// "nullptr" is where we could specify a damage type if we make multiple (blunt, piercing, fire, etc).
		FPointDamageEvent DamageEvent(WeaponDamage, Hit, Direction, nullptr);

		HitActor->TakeDamage(
			WeaponDamage,	 // Amount of damage.
			DamageEvent,	 // Damage event we made.
			OwnerController, // Who instigated the damage (the controller who attacked with this Weapon).
			this			 // What did the damage (this Weapon).
			);
	}
}

// -------------------------------------
/// Do a line trace from starting point in given direction based on MaxBasicAttackRange. \n
/// Hit results to OutHit. \n\n Return if hit was successful.
bool AWeapon::BounceLineTrace(const FVector Start, const FVector Direction, FHitResult& OutHit)
{
	// Then we travel out in a line from our ViewLocation in our ViewDirection, out to our max range.
	FVector EndPoint = Start + Direction * MaxBasicAttackRange;

	FCollisionQueryParams HitParameters;
	// If indicating CollisionQueryParams, make sure we keep trace to complex.
	HitParameters.bTraceComplex = true;
	// Ensure we don't hit ourselves or our weapon.
	HitParameters.AddIgnoredActor(this);
	HitParameters.AddIgnoredActor(GetOwner());

	bool Hit = GetWorld()->LineTraceSingleByChannel(
		OUT OutHit,			   // Out hit info.
		Start,		           // Start.
		EndPoint,			   // End.
		ECC_GameTraceChannel1, // Our "Bullet" channel is here, as shown in Config/DefaultEngine.ini
		HitParameters		   // Collision Parameters (make sure we don't shoot ourselves).
		);

	return Hit;
}

// -------------------------------------
/// Update our ricochet bounce rotation, direction and start location with new reflection.
void AWeapon::ReflectForNextBounce(const FHitResult Hit, const FVector Direction)
{
	// Ricochet info that needs to be updated for additional BounceImpact() calls.
	FVector MirrorImpact = FMath::GetReflectionVector(Direction, Hit.ImpactNormal);
	RicochetRotation = MirrorImpact.Rotation();
	RicochetDirection = RicochetRotation.Vector();
	StartLocation = Hit.Location;
}

// -------------------------------------
/// Subsequent recursive calls to BounceImpact() with Timer Delegate. \n\n
/// End bounces once maximum bounce count has been reached.
void AWeapon::DoNextBounceImpact()
{
	// Setup timer delegate so we can make a sort of delayed ricochet bounce by calling this again.
	// Recursive calls means updating the parameters/values with the new bind.
	RicochetDelegate.BindUFunction(this, FName("BounceImpact"), StartLocation, RicochetDirection);

	// Our recursive loop and base case.
	if (RicochetBounces < RicochetMaxBounces) {
		RicochetBounces += 1;
		// Slowly speed up the bounces as we go on.
		RicochetBounceDelay *= RicochetBounceSpeedGrowthFactor;
		// Call this function again after a short delay, with updated parameters.
		GetWorld()->GetTimerManager().SetTimer(RicochetTimerHandle, RicochetDelegate, RicochetBounceDelay, false);
	}
	else if (RicochetBounces == RicochetMaxBounces) {
		// TODO: Different sound effect.
		// TODO: Different Explosion effect.
	}
}
