// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

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

	CameraSpringArm = GetComponentByClass(USpringArmComponent::StaticClass());

}

// --------------------------------------------------------------
// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// --------------------------------------------------------------
// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("SwapShoulder"), this, &AShooterCharacter::MoveFoward);

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

//
/// Swap over-the-shoulder view between left and right.
void AShooterCharacter::SwapShoulder()
{

}
