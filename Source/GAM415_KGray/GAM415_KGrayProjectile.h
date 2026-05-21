// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAM415_KGrayProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UMaterialInterface;
class UNiagaraSystem; // [Week 3] Forward declaration for the Niagara System

UCLASS(config = Game)
class AGAM415_KGrayProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AGAM415_KGrayProjectile();

	// [Week 2] Exposing the base decal material to the editor so we can assign M_DecalBase.
	// We keep the actual instantiation and parameter manipulation strictly in C++.
	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* DecalMaterialBase;

	// [Week 3] Exposing the Niagara Particle System to the editor so we can assign NS_Splat.
	UPROPERTY(EditAnywhere, Category = "Particles")
	UNiagaraSystem* SplatParticleSystem;

	// [Week 2] Storing the randomized color generated at spawn so we can pass the exact 
	// same FLinearColor to the decal and particle system upon impact.
	FLinearColor RandomProjectileColor;

protected:
	// [Week 2] Overriding BeginPlay to set the projectile color as soon as it spawns.
	virtual void BeginPlay() override;

public:
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};