// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "ShooterCharacter.generated.h"

// ---------------------------------------------------------------
// Forward Declarations.
class AWeapon;
class USpringArmComponent;

// ---------------------------------------------------------------
UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/// Sets default values for this character's properties.
	AShooterCharacter();

	/** Apply damage to this ShooterCharacter. -7MD
	* @see https://www.unrealengine.com/blog/damage-in-ue4
	* @param DamageAmount		How much damage to apply
	* @param DamageEvent		Data package that fully describes the damage received.
	* @param EventInstigator	The Controller responsible for the damage.
	* @param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	* @return					The amount of damage actually applied. */
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
		) override;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// BlueprintPure functions are purely nodes, they don't have execution pins.
	// This means running this function doesn't change anything anywhere, it only returns stuff.
	// Because of that, making IsDead() const makes sense since it won't change anything in this class.

	/// Return if the character is dead or not.
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

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
	// Attack stuff.
	void AttackBasic();
	/// Weapon class selected in editor.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY()
	AWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, Category="Health")
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, Category="Health")
	float Health;
};
