// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415_KGrayProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
// [Week 2] Added includes required for GameplayStatics (spawning decals) and Dynamic Materials.
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"
// [Week 3] Added includes for Niagara spawning and component manipulation.
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
// [Week 4] Added include to allow the projectile to interact with our custom procedural terrain.
#include "ProceduralTerrain.h"

AGAM415_KGrayProjectile::AGAM415_KGrayProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415_KGrayProjectile::OnHit);

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

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
		UMaterialInstanceDynamic* DynamicMat = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMat)
		{
			DynamicMat->SetVectorParameterValue(FName("Color"), RandomProjectileColor);
		}
	}
}

void AGAM415_KGrayProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// [Week 2] Handle the Decal spawning and parameter randomization entirely in C++.
	if (DecalMaterialBase && OtherActor != nullptr && OtherActor != this)
	{
		UMaterialInstanceDynamic* DecalDMI = UMaterialInstanceDynamic::Create(DecalMaterialBase, this);
		if (DecalDMI)
		{
			DecalDMI->SetVectorParameterValue(FName("Color"), RandomProjectileColor);

			int32 RandomFrame = FMath::RandRange(0, 3);
			float AnimationPhase = RandomFrame / 4.0f;
			DecalDMI->SetScalarParameterValue(FName("Frame"), AnimationPhase);

			UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(), DecalDMI, FVector(15.f, 30.f, 30.f),
				Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), 10.f
			);
		}
	}

	// [Week 3] Handle Niagara Particle System spawning and color synchronization.
	// We perform a check to ensure SplatParticleSystem is valid so the engine doesn't crash.
	if (SplatParticleSystem && OtherActor != nullptr && OtherActor != this)
	{
		// [Week 3] Spawn the Niagara system at the hit location.
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			SplatParticleSystem,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation()
		);

		if (NiagaraComp)
		{
			// [Week 3] Set the user parameter "RandColor" to match the projectile's random color.
			NiagaraComp->SetVariableLinearColor(FName("RandColor"), RandomProjectileColor);
		}
	}

	// [Week 4] Perform a Raytrace (LineTrace) straight down from the player's projectile impact.
	FHitResult TraceHit;
	FVector StartTrace = Hit.ImpactPoint + FVector(0.f, 0.f, 50.f); // Start slightly above the impact
	FVector EndTrace = StartTrace + FVector(0.f, 0.f, -200.f); // Trace downwards

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the projectile itself

	// If the raytrace hits our new Procedural Terrain, alter it!
	if (GetWorld()->LineTraceSingleByChannel(TraceHit, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{
		AProceduralTerrain* HitTerrain = Cast<AProceduralTerrain>(TraceHit.GetActor());
		if (HitTerrain)
		{
			// [Week 4] Carve a crater into the terrain at the exact raytrace impact point.
			HitTerrain->AlterTerrain(TraceHit.ImpactPoint, 250.f, 150.f);
		}
	}

	if (OtherActor != nullptr && OtherActor != this)
	{
		Destroy();
	}
}