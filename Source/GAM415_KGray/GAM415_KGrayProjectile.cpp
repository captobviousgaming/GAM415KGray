// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_KGrayProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
// [Week 2] Added includes required for GameplayStatics (spawning decals) and Dynamic Materials.
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"

AGAM415_KGrayProjectile::AGAM415_KGrayProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415_KGrayProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AGAM415_KGrayProjectile::BeginPlay()
{
	Super::BeginPlay();

	// [Week 2] Generate a completely random color for the projectile mesh upon spawning.
	RandomProjectileColor = FLinearColor::MakeRandomColor();

	// [Week 2] Find the Static Mesh Component that is attached via the Blueprint hierarchy.
	UStaticMeshComponent* MeshComp = FindComponentByClass<UStaticMeshComponent>();

	if (MeshComp)
	{
		// [Week 2] Create a Dynamic Material Instance (DMI) via C++ for the projectile's mesh. 
		// This allows us to alter the "Color" parameter at runtime strictly via code.
		UMaterialInstanceDynamic* DynamicMat = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMat)
		{
			DynamicMat->SetVectorParameterValue(FName("Color"), RandomProjectileColor);
		}
	}
}

void AGAM415_KGrayProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add physical impulse if we hit a physics object
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// [Week 2] Handle the Decal spawning and parameter randomization entirely in C++.
	// We run this outside the physics check so the decal still spawns on static walls.
	if (DecalMaterialBase && OtherActor != nullptr && OtherActor != this)
	{
		// [Week 2] Create a DMI for the decal so we can manipulate it independently.
		UMaterialInstanceDynamic* DecalDMI = UMaterialInstanceDynamic::Create(DecalMaterialBase, this);
		if (DecalDMI)
		{
			// [Week 2] Match the decal color to the saved projectile mesh color.
			DecalDMI->SetVectorParameterValue(FName("Color"), RandomProjectileColor);

			// [Week 2] Randomize the splat texture.
			// The Flipbook node expects a percentage between 0.0 and 1.0.
			// Dividing our random integer (0-3) by 4.0 gives us exactly 0.0, 0.25, 0.5, or 0.75.
			int32 RandomFrame = FMath::RandRange(0, 3);
			float AnimationPhase = RandomFrame / 4.0f;
			DecalDMI->SetScalarParameterValue(FName("Frame"), AnimationPhase);

			// [Week 2] Spawn the randomized decal at the exact hit location and rotation.
			UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				DecalDMI,
				FVector(15.f, 30.f, 30.f), // Depth, Width, Height of the decal box
				Hit.ImpactPoint,
				Hit.ImpactNormal.Rotation(),
				10.f // Decal lifespan in seconds before it is destroyed
			);
		}
	}

	// Destroy the projectile whether it hit a physics object or a static wall
	if (OtherActor != nullptr && OtherActor != this)
	{
		Destroy();
	}
}