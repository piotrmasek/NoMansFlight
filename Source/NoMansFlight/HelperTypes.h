#pragma once
#include "HelperTypes.generated.h"

USTRUCT()
struct FTerrainLayer
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		float MinHeight;

	UPROPERTY(EditAnywhere)
		float MaxHeight;

	UPROPERTY(EditAnywhere)
		FColor Color;

	FTerrainLayer()
	{
		MinHeight = 0.f;
		MaxHeight = 0.f;
		Color = FColor{};
	}
};


USTRUCT()
struct FNoiseParams
{
	GENERATED_BODY()
public:

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 1))
		int ResX;

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 1))
		int ResY;

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 0.0001f))
		float Scale;

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 0))
		int32 Octaves;

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 0.f, UIMax = 1.f))
		float Persistence;

	UPROPERTY(Category = Noise, EditAnywhere, Config, Meta = (UIMin = 1))
		float Lacunarity;

	UPROPERTY(Category = Noise, EditAnywhere, Config)
		FVector2D Offset;

	UPROPERTY(Category = Noise, EditAnywhere, Config)
		int32 Seed;

	FNoiseParams()
	{
		ResX = ResY = 200;
		Scale = 25.f;
		Octaves = 4;
		Persistence = 0.5f;
		Lacunarity = 2;
		Seed = FMath::Rand();
	}
};

USTRUCT()
struct FMeshParams
{
	GENERATED_BODY()
public:
	UPROPERTY(Category = Mesh, EditAnywhere, Config)
		float HeightMultiplier;

	UPROPERTY(Category = Mesh, EditAnywhere)
		TArray<FTerrainLayer> TerrainLayers;

	UPROPERTY(Category = Mesh, EditAnywhere)
		UMaterialInstance * Material;

	FMeshParams()
	{
		HeightMultiplier = 25.f;
	}
};

