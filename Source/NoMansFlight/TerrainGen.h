// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"


#include "TerrainGen.generated.h"


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

UCLASS()
class NOMANSFLIGHT_API ATerrainGen : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATerrainGen();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//UFUNCTION(BlueprintCallable)
	static TArray<float> GenerateHeightMap(int32 SizeX, int32 SizeY, float Scale, int32 Octaves, float Persistence, float Lacunarity, FVector2D Offset, int32 Seed);
	
	void CreateMesh(const TArray<float>& HeightMap);

	UPROPERTY(EditAnywhere)
	FVector ChunkSize;
	
	UPROPERTY(EditAnywhere, Meta = (UIMin = 1))
	int SizeX;

	UPROPERTY(EditAnywhere, Meta = (UIMin = 1))
	int SizeY;

	UPROPERTY(EditAnywhere, Meta = (UIMin = 0.0001f))
	float Scale;

	UPROPERTY(EditAnywhere, Meta = (UIMin = 0))
	int32 Octaves;

	UPROPERTY(EditAnywhere, Meta = (UIMin = 0.f, UIMax = 1.f))
	float Persistence;

	UPROPERTY(EditAnywhere, Meta = (UIMin = 1))
	float Lacunarity;

	UPROPERTY(EditAnywhere)
	FVector2D Offset;

	UPROPERTY(EditAnywhere)
	int32 Seed;

	UPROPERTY(EditAnywhere)
	float HeightMultiplier;

	UPROPERTY(EditAnywhere)
	TArray<FTerrainLayer> TerrainLayers;

	UPROPERTY(EditAnywhere)
	UMaterial * Material;

	UPROPERTY(EditAnywhere)
	URuntimeMeshComponent* RuntimeMesh;

private:

};
