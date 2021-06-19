// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


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
void AWeapon::AttackBasic()
{
	UGameplayStatics::SpawnEmitterAttached(
		WeaponFlash,		       // Emitter.
		DragonSwordMesh,	       // Component to attach to.
		TEXT("WeaponFlashSocket")  // Bone/Socket to attach to.
		// FVector Location,
		// FRotator Rotation,
		// FVector Scale,
		// EAttachLocation::Type LocationType,
		// bool bAutoDestroy,
		// EPSCPoolMethod PoolingMethod,
		// bool bAutoActivateSystem
		);

	// This is completely overkill but I wanted to know if I could do it.
	// Getting the FOV from the player camera, for the sake of DrawDebugCamera.
	UCameraComponent* PlayerCam = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));

	DrawDebugCamera(
		GetWorld(),
		PlayerCam->GetComponentLocation(),
		PlayerCam->GetComponentRotation(),
		PlayerCam->FieldOfView,
		1,
		FColor::Green,
		false,
		2);
}

// ----------------------------------------------------------
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// ----------------------------------------------------------
// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
