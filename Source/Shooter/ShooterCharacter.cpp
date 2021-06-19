// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Weapon.h"

// --------------------------------------------------------------
// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set to false if tick is not needed.
	PrimaryActorTick.bCanEverTick = true;

}

// --------------------------------------------------------------
// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the spring arm so we can shoulder swap.
	CameraSpringArm = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass()));
	// As long as the spring arm in the BP is centered around the player,
	// we can just multiply it's existing offset by -1 to swap it.
	RightShoulderOffset = CameraSpringArm->SocketOffset.Y;
	LeftShoulderOffset = RightShoulderOffset * -1;

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

}

// --------------------------------------------------------------
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smooth shoulder swapping.
	MoveToShoulder(DeltaTime);
}

// --------------------------------------------------------------
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

	PlayerInputComponent->BindAction(TEXT("SwapShoulder"), IE_Pressed, this, &AShooterCharacter::SwapShoulder);
	PlayerInputComponent->BindAction(TEXT("AttackBasic"), IE_Pressed, this, &AShooterCharacter::AttackBasic);
}

// --------------------------------------------------------------
void AShooterCharacter::MoveFoward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

// --------------------------------------------------------------
void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

// --------------------------------------------------------------
/// Framerate-independent implementation of AddControllerPitchInput, for joysticks/axis.
void AShooterCharacter::LookUpRate(float AxisValue)
{
	// Axis * Rotation rate would give us the speed,
	// but we need the distance for this to be framerate independent.
	// To get the distance, you multiply by time elapsed as well.
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

// --------------------------------------------------------------
/// Framerate-independent implementation of AddControllerYawInput, for joysticks/axis.
void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

// --------------------------------------------------------------
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

// --------------------------------------------------------------
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

// --------------------------------------------------------------
/// Perform a basic attack with the currently held weapon.
void AShooterCharacter::AttackBasic()
{
	Weapon->AttackBasic();
}
