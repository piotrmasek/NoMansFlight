// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RuntimeMeshComponent.h"
#include "TerrainChunk.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOMANSFLIGHT_API UTerrainChunk : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTerrainChunk();

	// Called when the game starts
	virtual void BeginPlay() override;

	void Init(int32 Id);
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	UPROPERTY(VisibleAnywhere)
	int32 Id;
	
	UPROPERTY(EditAnywhere)
	FIntVector ChunkCoord;

	UPROPERTY(EditAnywhere)
	FVector MeshScale;

	UPROPERTY(EditAnywhere)
	class URuntimeMeshComponent* RuntimeMesh;
private:
	UPROPERTY()
	class ATerrainGen* TerrainGen;



	


	
public:
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

};
