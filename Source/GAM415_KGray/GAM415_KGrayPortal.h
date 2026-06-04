// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAM415_KGrayPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class GAM415_KGRAY_API AGAM415_KGrayPortal : public AActor
{
	GENERATED_BODY()

public:
	AGAM415_KGrayPortal();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// [Week 5] The visual frame and window mesh of the portal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UStaticMeshComponent* PortalMesh;

	// [Week 5] The trigger volume to detect player overlap for teleportation.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UBoxComponent* TeleportVolume;

	// [Week 5] The scene capture component that calculates depth and acts as the live feed camera.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	USceneCaptureComponent2D* SceneCapture;

	// [Week 5] A reference to the destination portal to link them together.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	AGAM415_KGrayPortal* LinkedPortal;

	// [Week 5] The specific render target this portal's camera will write to.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	UTextureRenderTarget2D* RenderTarget;

	// [Week 5] Function to handle the teleportation logic when the player walks into the portal.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// [Week 5] Flag to prevent infinite teleportation loops between linked portals.
	bool bIsTeleporting;

private:
	// [Week 5] Logic to update camera rotation and distance to simulate depth on the render target.
	void UpdateSceneCapture();
};