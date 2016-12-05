// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HelperTypes.h"


#include "TerrainGen.generated.h"

UCLASS(config=TerrainGen)
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
	static void GenerateHeightMap(TArray<float>& OutHeightMap, int32 SizeX, int32 SizeY, float Scale, int32 Octaves, float Persistence, float Lacunarity, FVector2D Offset, int32 Seed);
	
	void CreateMesh(const TArray<float>& HeightMap);

	UPROPERTY(EditAnywhere)
	FVector ChunkSize;

	UPROPERTY(EditAnywhere)
	int32 ChunkVisibilityRange;
	

	UPROPERTY(Category = Noise, EditAnywhere, Config)
	FNoiseParams NoiseParams;

	UPROPERTY(Category = Mesh, EditAnywhere, Config)
	FMeshParams MeshParams;

	UPROPERTY(EditAnywhere)
	class URuntimeMeshComponent* RuntimeMesh;

	UPROPERTY(EditAnywhere)
	TArray<class UTerrainChunk*> TerrainChunks;

	UPROPERTY(EditAnywhere)
	bool bUpdateChunks;

private:
	void UpdateChunks();
	void CreateChunk(FIntVector ChunkCoord);
	void RemoveChunk(class UTerrainChunk* ChunkToRemove);
	
};
