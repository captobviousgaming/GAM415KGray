// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralTerrain.h"
// [Fix] Include the Kismet library to auto-calculate our lighting normals.
#include "KismetProceduralMeshLibrary.h" 

AProceduralTerrain::AProceduralTerrain()
{
	PrimaryActorTick.bCanEverTick = false;

	// [Week 4] Initialize the procedural mesh component and set it as the root.
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = ProceduralMesh;
	ProceduralMesh->bUseAsyncCooking = true;

	// Ensure the procedural mesh generates standard collision and blocks Raytraces!
	ProceduralMesh->bUseComplexAsSimpleCollision = true;
	ProceduralMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AProceduralTerrain::BeginPlay()
{
	Super::BeginPlay();
	GenerateTerrain();
}

void AProceduralTerrain::GenerateTerrain()
{
	// [Week 4] Loop through the dynamic XSize and YSize variables to create the grid.
	for (int32 Y = 0; Y <= YSize; ++Y)
	{
		for (int32 X = 0; X <= XSize; ++X)
		{
			// [Week 4] Utilize Perlin Noise to generate organic topology (height/Z-axis).
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale, Y * NoiseScale)) * ZMultiplier;

			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X, Y)); // Set UV mapping for materials
		}
	}

	// [Week 4] Define the triangles to connect our vertices together.
	for (int32 Y = 0; Y < YSize; ++Y)
	{
		for (int32 X = 0; X < XSize; ++X)
		{
			int32 TopLeft = (Y * (XSize + 1)) + X;
			int32 BottomLeft = ((Y + 1) * (XSize + 1)) + X;
			int32 TopRight = TopLeft + 1;
			int32 BottomRight = BottomLeft + 1;

			// [Fix] RESTORED your original, correct winding order so the mesh faces UP!
			Triangles.Add(TopLeft);
			Triangles.Add(BottomLeft);
			Triangles.Add(TopRight);

			Triangles.Add(TopRight);
			Triangles.Add(BottomLeft);
			Triangles.Add(BottomRight);
		}
	}

	// Create arrays to hold our lighting data.
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	// Auto-calculate the normals and tangents so the terrain reacts to light and casts shadows!
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

	// [Week 4] Command the API to build the physical mesh in the game world, passing in our new Normals.
	ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), Tangents, true);
}

void AProceduralTerrain::AlterTerrain(FVector HitLocation, float Radius, float Depth)
{
	bool bMeshModified = false;

	// [Week 4] Loop through all vertices to see which fall within the raytrace impact radius.
	for (int32 i = 0; i < Vertices.Num(); i++)
	{
		// Convert the local vertex position to world space for accurate comparison.
		FVector WorldVert = GetTransform().TransformPosition(Vertices[i]);

		if (FVector::Dist(WorldVert, HitLocation) < Radius)
		{
			// Deform the terrain downwards, creating a crater.
			Vertices[i].Z -= Depth;
			bMeshModified = true;
		}
	}

	if (bMeshModified)
	{
		// We must recalculate the normals again when the mesh changes so the shadows update!
		TArray<FVector> UpdatedNormals;
		TArray<FProcMeshTangent> UpdatedTangents;
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, UpdatedNormals, UpdatedTangents);

		// [Week 4] Send the altered vertices back to the graphics API to redraw the terrain dynamically.
		ProceduralMesh->UpdateMeshSection_LinearColor(0, Vertices, UpdatedNormals, UV0, TArray<FLinearColor>(), UpdatedTangents);
	}
}