// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	void AttackBasic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void BounceImpact(FVector Start, FVector Direction);
	UFUNCTION()
	void DoDamage(FHitResult Hit, FVector Direction);

	FVector StartLocation;
	FRotator StartRotation;
	FVector StartDirection;
	FRotator RicochetRotation;
	FVector RicochetDirection;
	FTimerHandle RicochetTimerHandle;
	FTimerDelegate RicochetDelegate;

	UPROPERTY()
	APawn* OwnerPawn;
	UPROPERTY()
	UCapsuleComponent* OwnerCapsule;
	UPROPERTY()
	AController* OwnerController;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* DragonSwordMesh;

	/// Enable to shoot yourself forward on the same Bounce trajectory as your projectile.
	UPROPERTY(EditAnywhere, Category="Nonsense")
	bool bToggleSelfRicochet = false;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* WeaponFlash;
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* HitImpactSound;
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* FinalHitExplosionSound;

	UPROPERTY(EditAnywhere, Category="Attacks")
	float WeaponDamage = 10;
	UPROPERTY(EditAnywhere, Category="Attacks")
	float MaxBasicAttackRange;
	UPROPERTY(EditAnywhere, Category="Attacks")
	FVector ImpactEffectScale;
	UPROPERTY(EditAnywhere, Category="Attacks")
	int32 RicochetMaxBounces = 200;
	UPROPERTY(EditAnywhere, Category="Attacks")
	float RicochetBounceStartDelay = 0.1f;
	UPROPERTY(EditAnywhere, Category="Attacks")
	float RicochetBounceSpeedGrowthFactor = 0.99f;
	float RicochetBounceDelay;

	int32 RicochetBounces = 0;
};
