// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Field/FieldSystemNodes.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


// ----------------------------------------------------------
// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	DragonSwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dragon Sword Skeletal Mesh"));
	DragonSwordMesh->SetupAttachment(Root);

}

// ----------------------------------------------------------
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	RicochetBounceDelay = RicochetBounceDelayDefault;
}

// ----------------------------------------------------------
// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ----------------------------------------------------------
void AWeapon::AttackBasic()
{
	RicochetBounceDelay = RicochetBounceDelayDefault;
	RicochetBounces = 0;

	// Attack Sound.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FinalHitExplosionSound, GetActorLocation(), FRotator::ZeroRotator);

	// Attack particle effect.
	UGameplayStatics::SpawnEmitterAttached(
		WeaponFlash,		       // Emitter.
		DragonSwordMesh,	       // Component to attach to.
		TEXT("WeaponFlashSocket")  // Bone/Socket to attach to.
		);

	// We want to shoot to where our view/camera is aiming.
	// To get our player view point from here, we need to access this weapon's owner's controller.
	// First we get the pawn.
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	// Then from the pawn we can get the controller.
	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController) return;
	// Then from the controller we can get the viewpoint.
	OwnerController->GetPlayerViewPoint(OUT StartLocation, OUT StartRotation);

	// Direction that points from the rotation.
	// We'll make this our first direction.
	RicochetDirection = StartRotation.Vector();

	// Start the ricochets!
	BounceImpact(StartLocation, RicochetDirection);
}

// ----------------------------------------------------------
/// Line trace to hit target.\n
/// Ricochet emitter rotation across hit surface.\n
///	Update StartDirection and RicochetDirection.
void AWeapon::BounceImpact(FVector Start, FVector Direction)
{
	// Then we travel out in a line from our ViewLocation in our ViewDirection, out to our max range.
	FVector EndPoint = Start + Direction * MaxBasicAttackRange;

	// Finally, do the LineTrace.
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(
		OUT Hit,			  // Out hit info.
		Start,		          // Start.
		EndPoint,			  // End.
		ECC_GameTraceChannel1 // Our "Bullet" channel is here, as shown in Config/DefaultEngine.ini
		);

	if (Hit.IsValidBlockingHit()) {
		StartLocation = Hit.Location;
		// Ricochet info!
		FVector MirrorImpact = FMath::GetReflectionVector(Direction, Hit.ImpactNormal);
		RicochetRotation = MirrorImpact.Rotation();
		RicochetDirection = RicochetRotation.Vector();

		// Spawn particles!
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),		   // World context.
			WeaponFlash,	   // Particle emitter.
			Hit.ImpactPoint,   // Location.
			RicochetRotation, // Rotation.
			ImpactEffectScale  // Scale.
		);

		// Play sound!
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitImpactSound, Hit.ImpactPoint, FRotator::ZeroRotator);

		// TODO: Have effect/emitter line travel from hand to impact point, then via bounces.
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Purple, false, 5);

		// Setup timer delegate so we can make a sort of delayed ricochet bounce by calling this again.
		RicochetDelegate.BindUFunction(this, FName("BounceImpact"), StartLocation, RicochetDirection);

		// Our recursive loop!
		if (RicochetBounces < RicochetMaxBounces) {
			RicochetBounces += 1;
			// Slowly speed up the bounces as we go on.
			RicochetBounceDelay *= RicochetBounceSpeedGrowthFactor;
			// Call this function again after a short delay, with updated parameters.
			GetWorld()->GetTimerManager().SetTimer(RicochetTimerHandle, RicochetDelegate, RicochetBounceDelay, false);
		}
		else if (RicochetBounces == RicochetMaxBounces) {
			// The final explosion after all the bounces are done.
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FinalHitExplosionSound, Hit.ImpactPoint, FRotator::ZeroRotator);
		}
	}
}
