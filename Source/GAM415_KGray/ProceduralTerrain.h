

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain.generated.h"

UCLASS()
class GAM415_KGRAY_API AProceduralTerrain : public AActor
{
	GENERATED_BODY()

public:
	AProceduralTerrain();

protected:
	virtual void BeginPlay() override;

public:
	// [Week 4] Editor variables to make the procedural terrain dynamic.
	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	int32 XSize = 20;

	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	int32 YSize = 20;

	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	float Scale = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	float NoiseScale = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Terrain Settings")
	float ZMultiplier = 300.0f;

	// [Week 4] The component that handles rendering our custom geometry.
	UPROPERTY(VisibleAnywhere, Category = "Terrain Settings")
	UProceduralMeshComponent* ProceduralMesh;

	// [Week 4] Function allowing the player to alter the terrain via raytrace impact.
	UFUNCTION(BlueprintCallable, Category = "Terrain")
	void AlterTerrain(FVector HitLocation, float Radius, float Depth);

private:
	// Data arrays required to build a 3D mesh from scratch.
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;

	void GenerateTerrain();
};