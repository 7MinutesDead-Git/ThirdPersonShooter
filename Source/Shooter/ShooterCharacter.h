// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

#include "ShooterCharacter.generated.h"

// ---------------------------------------------------------------
class AWeapon;

// ---------------------------------------------------------------
UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// ----------------------------------
	// Control.
	void MoveFoward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);

	UPROPERTY(EditAnywhere, Category="View")
	float RotationRate = 100;

	// ----------------------------------
	// Camera.
	UPROPERTY()
	USpringArmComponent* CameraSpringArm;

	void SwapShoulder();
	void MoveToShoulder(float DeltaTime);

	bool bRightShoulder = true;
	float RightShoulderOffset;
	float LeftShoulderOffset;
	UPROPERTY(EditAnywhere, Category="View")
	float ShoulderSwapSpeed = 10;

	// ----------------------------------
	void AttackBasic();
	/// Weapon class selected in editor.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY()
	AWeapon* Weapon;
};
