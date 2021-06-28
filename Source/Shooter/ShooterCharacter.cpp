// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "SurvivalGameMode.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// -----------------------------------------------------------------------------------
// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set to false if tick is not needed.
	PrimaryActorTick.bCanEverTick = true;

}

// -----------------------------------------------------------------------------------
// Called when the game starts or when spawned.
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = GetWorldSettings();

	// Camera setup -------------------------------------------
	// Get reference to the spring arm so we can shoulder swap.
	CameraSpringArm = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass()));
	CameraSpringArmLagSpeedDefault = CameraSpringArm->CameraLagSpeed;
	// As long as the spring arm in the BP is centered around the player,
	// we can just multiply it's existing offset by -1 to swap it.
	RightShoulderOffset = CameraSpringArm->SocketOffset.Y;
	LeftShoulderOffset = RightShoulderOffset * -1;

	// Weapon setup -------------------------------------------
	// Since our mesh has a sword already, we need to hide it.
	// It's attached to the bone "weapon_r" in the skeleton.
	// We made a new socket in it's place in the editor named WeaponSocket.
	GetMesh()->HideBoneByName(TEXT("weapon_r"), PBO_None);
	// Create our custom weapon to equip.
	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	// Attach it to this mesh's WeaponSocket (our player), and keep relative transform to bone.
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	// Setting ownership here is relevant for multiplayer and damage.
	// This means the weapon is also aware of the character, so references can be retrieved too!
	Weapon->SetOwner(this);

	// Player resources setup. --------------------------------
	Health = MaxHealth;
	DashResource = DashResourceMax;

}

// -----------------------------------------------------------------------------------
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smooth shoulder swapping.
	MoveToShoulder(DeltaTime);
	// Replenish Dash resource over time, clamped between 0 and DashResourceMax.
	DashResource = FMath::Clamp<float>(DashResource + (DashRefillRate * DeltaTime), 0, DashResourceMax);
}

// -----------------------------------------------------------------------------------
// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveFoward);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &AShooterCharacter::MoveRight);

	// Inherited from APawn.
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShooterCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AShooterCharacter::AddControllerYawInput);

	// Separate needed for joysticks/axis, because they are affected by framerate.
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);

	// Inherited from ACharacter (child of APawn).
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AShooterCharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &AShooterCharacter::Dash);

	PlayerInputComponent->BindAction(TEXT("SwapShoulder"), IE_Pressed, this, &AShooterCharacter::SwapShoulder);
	PlayerInputComponent->BindAction(TEXT("AttackBasic"), IE_Pressed, this, &AShooterCharacter::AttackBasic);
}

// -----------------------------------------------------------------------------------
void AShooterCharacter::MoveFoward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);

	// So the character doesn't disappear from view when backpedaling or teleporting backwards too fast.
	if (AxisValue < 0) {
		CameraSpringArm->CameraLagSpeed = CameraBackpedalSpeed;
	}
	else {
		CameraSpringArm->CameraLagSpeed = CameraSpringArmLagSpeedDefault;
	}
}

// -----------------------------------------------------------------------------------
void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

// -----------------------------------------------------------------------------------
/// Framerate-independent implementation of AddControllerPitchInput, for joysticks/axis.
void AShooterCharacter::LookUpRate(float AxisValue)
{
	// Axis * Rotation rate would give us the speed,
	// but we need the distance for this to be framerate independent.
	// To get the distance, you multiply by time elapsed as well.
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

// -----------------------------------------------------------------------------------
/// Framerate-independent implementation of AddControllerYawInput, for joysticks/axis.
void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

// -----------------------------------------------------------------------------------
/// Spawn particles related to movement, both on character and at origin of movement.
void AShooterCharacter::SpawnMovementParticles(const FVector Direction, const float ForwardAxisInput, const float StrafeAxisInput)
{
	const FVector LocationOffset = DashParticleEffectLocationOffset;

	if (ForwardAxisInput != 0 || StrafeAxisInput != 0) {
		// Needed to counteract mesh's own rotation when attached.
		DashParticleAttachedRotation = Direction.Rotation() - GetActorRotation();
		DashParticleRotation = Direction.Rotation();
		// This resolves strange offset only when attaching to Mesh. I don't know why yet.
		DashParticleAttachedRotation.Yaw += 90;
	}
	else {
		// If we're dashing from a standstill then we can't get direction from velocity.
		DashParticleAttachedRotation = GetActorForwardVector().Rotation() - GetActorRotation();
		DashParticleAttachedRotation.Yaw += 90;
		DashParticleRotation = GetActorForwardVector().Rotation();
	}

	// Play dash particle on ourselves...
	UGameplayStatics::SpawnEmitterAttached(
		DashParticleEffect,			  // Particle to spawn.
		GetMesh(),				  	  // Attach to our mesh.
		NAME_None,				 	  // Bone name to attach to.
		LocationOffset,		          // Relative Location.
		DashParticleAttachedRotation, // Relative Rotation.
		DashParticleEffectScale,	  // Scale.
		EAttachLocation::SnapToTarget // Type of Location offset.
		);

	// And where we just were.
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),							 // World Context.
		DashParticleEffect,					 // Particle emitter.
		GetActorLocation() + LocationOffset, // Spawn at this location.
		DashParticleRotation     			 // Rotate the effect to face towards our velocity.
		);
}

// -----------------------------------------------------------------------------------
/// Dash/teleport in intended move direction based on WASD input, or forward if standing still.
/// Very Tracer baby.
void AShooterCharacter::Dash()
{
	if (DashResource >= DashCost) {
		DashSlowTime();

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DashSound, GetActorLocation());

		const float ForwardValue = GetInputAxisValue("MoveForward");
		const float StrafeValue = GetInputAxisValue("Strafe");
		const FVector ForwardVector = GetActorForwardVector();
		const FVector StrafeVector = GetActorRightVector();

		const FVector Direction = (ForwardVector * ForwardValue) + (StrafeVector * StrafeValue);
		const FVector Location = GetActorLocation();

		if (ForwardValue != 0 || StrafeValue != 0) {
			DashDestination = Location + Direction * DashDistance;
		}
		else {
			// Dash forward if not moving, so we at least do something when we hit Dash input.
			DashDestination = Location + ForwardVector * DashDistance;
		}

		SpawnMovementParticles(Direction, ForwardValue, StrafeValue);
		PlayAnimMontage(DashAnimation);

		// Make sure we sweep check ("true") so we don't teleport through walls.
		SetActorLocation(DashDestination, true);
	}
	// Make sure we don't drain dash resource when we try to dash without having enough to do so!
	if (DashResource >= DashCost)
		DashResource -= DashCost;
}

// -----------------------------------------------------------------------------------
float AShooterCharacter::GetDashResourceAsPercentage() const
{
	return DashResource / DashResourceMax;
}

// -----------------------------------------------------------------------------------
/// Slow motion when we dash. Return to normal time after delay.
void AShooterCharacter::DashSlowTime()
{
	if (WorldSettings) {
		WorldSettings->SetTimeDilation(DashTimeDilation);
		// Resume normal speed after short delay.
		GetWorldTimerManager().SetTimer(DashSlowTimeTimer, this, &AShooterCharacter::ReturnToNormalTime, DashTimeDilationLength);
	}
}

// -----------------------------------------------------------------------------------
/// Reset time dilation to 1.
void AShooterCharacter::ReturnToNormalTime() const
{
	WorldSettings->SetTimeDilation(1);
}

// -----------------------------------------------------------------------------------
// Overriding to do extra stuff when we jump.
void AShooterCharacter::Jump()
{
	Super::Jump();

	PlayAnimMontage(JumpAnimation);
	// Ensure we don't continue to spawn particles when we can't double/triple jump anymore.
	if (JumpCurrentCount < JumpMaxCount && JumpCurrentCount != 0) {
		// TODO: Spawn jump particles.
	}
}

// -----------------------------------------------------------------------------------
/// Swap over-the-shoulder view between left and right, toggled by input.
void AShooterCharacter::SwapShoulder()
{
	if (bRightShoulder) {
		// TODO: Play swish sound when we swap.
		bRightShoulder = false;
	}
	else {
		// TODO: Another another slightly different when swapping back.
		bRightShoulder = true;
	}
}

// -----------------------------------------------------------------------------------
/// Interpolate towards desired shoulder offset.
void AShooterCharacter::MoveToShoulder(float DeltaTime)
{
	float CurrentPos = CameraSpringArm->SocketOffset.Y;

	if (bRightShoulder) {
		CameraSpringArm->SocketOffset.Y = FMath::FInterpTo(CurrentPos, RightShoulderOffset, DeltaTime, ShoulderSwapSpeed);
	}
	else {
		CameraSpringArm->SocketOffset.Y = FMath::FInterpTo(CurrentPos, LeftShoulderOffset, DeltaTime, ShoulderSwapSpeed);
	}
}

// -----------------------------------------------------------------------------------
/// Perform a basic attack with the currently held weapon.
void AShooterCharacter::AttackBasic()
{
	Weapon->AttackBasic();
	Attacking = true;

	PlayAnimMontage(AttackAnimation);

	FTimerHandle StopAttackTimer;
	GetWorld()->GetTimerManager().SetTimer(
		OUT StopAttackTimer,
		this,
		&AShooterCharacter::StopAttacking,
		AttackTimeLength,
		false
		);
}

// -----------------------------------------------------------------------------------
/// Set Attacking to false and stop animations.
void AShooterCharacter::StopAttacking()
{
	Attacking = false;
	StopAnimMontage(AttackAnimation);
}

// -----------------------------------------------------------------------------------
float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Super calling parent TakeDamage first, passing in our parameters.
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Apply damage to our Health (not going lower than 0 or higher than MaxHealth.
	Health = FMath::Clamp<float>(Health-DamageApplied, 0, MaxHealth);

	PlayAnimMontage(HitAnimation);

	if (IsDead()) {
		// Let GameMode know this pawn died.
		AShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AShooterGameModeBase>();
		if (GameMode) {
			GameMode->PawnKilled(this);
		}
		// If we die, detach the controller and disable collision of the capsule.
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Swap to ragdoll. TODO: Fix ragdoll weirdness.
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->bPauseAnims = true;
		Destroy();
	}

	// Return adjusted damage value if needed.
	return DamageApplied;
}

// -----------------------------------------------------------------------------------
bool AShooterCharacter::IsDead() const
{
	if (Health <= 0) {
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------------
bool AShooterCharacter::IsAttacking() const
{
	return Attacking;
}

// -----------------------------------------------------------------------------------
bool AShooterCharacter::IsDashing() const
{
	return Dashing;
}

// -----------------------------------------------------------------------------------
bool AShooterCharacter::IsJumping() const
{
	return bPressedJump;
}

// -----------------------------------------------------------------------------------
float AShooterCharacter::GetCurrentHealthAsPercentage() const
{
	return Health / MaxHealth;
}
