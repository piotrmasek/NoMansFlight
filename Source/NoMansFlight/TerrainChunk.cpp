// Fill out your copyright notice in the Description page of Project Settings.

#include "NoMansFlight.h"
#include "TerrainChunk.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshLibrary.h"
#include "TerrainGen.h"


// Sets default values for this component's properties
UTerrainChunk::UTerrainChunk()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	
	Id = -1;
	// ...

	//RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));

}


// Called when the game starts
void UTerrainChunk::BeginPlay()
{
	Super::BeginPlay();
	
	
	// ...
}


void UTerrainChunk::Init(int32 ChunkId)
{
	Id = ChunkId;
	
	TerrainGen = Cast<ATerrainGen>(GetOwner());
	check(TerrainGen);
	if (TerrainGen)
	{
		RuntimeMesh = TerrainGen->RuntimeMesh;
	}
	check(RuntimeMesh);
	if (RuntimeMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("RuntimeMesh in parent is null!"));
	}

	NoiseParams = TerrainGen->NoiseParams;
	MeshParams = TerrainGen->MeshParams;
	
	SetWorldLocation(FVector{ ChunkCoord.X * ChunkSize.X, ChunkCoord.Y * ChunkSize.Y, 0.f });

	
	
		ATerrainGen::GenerateHeightMap(HeightMap,
		NoiseParams.ResX,
		NoiseParams.ResY,
		NoiseParams.Scale,
		NoiseParams.Octaves,
		NoiseParams.Persistence,
		NoiseParams.Lacunarity,
		FVector2D{ GetComponentLocation() },
		NoiseParams.Seed);
	

		GenerateMesh();
}

// Called every frame
void UTerrainChunk::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UTerrainChunk::GenerateMesh()
{
	TArray<FRuntimeMeshVertexSimple> Vertices;
	TArray<int32> Triangles;
	int32 ResX = NoiseParams.ResX;
	int32 ResY = NoiseParams.ResY;

	Vertices.Reserve(ResX * ResY);

	FVector MeshScale = ChunkSize;
	MeshScale.X /= ResX;
	MeshScale.Y /= ResY;
	MeshScale.Z /= MeshParams.HeightMultiplier;

	RuntimeMesh->SetRelativeScale3D(MeshScale);

	float TopLeftX = (ResX - 1) / 2.f + ChunkCoord.X * ResX;
	float TopLeftY = (ResY - 1) / -2.f + ChunkCoord.Y * ResY;

	int vertexIndex = 0;
	for (int y = 0; y < ResY; ++y)
		for (int x = 0; x < ResX; ++x)
		{
			FRuntimeMeshVertexSimple Vertex;

			FVector Position;
			Position.X = TopLeftX - x;
			Position.Y = TopLeftY + y;
			Position.Z = HeightMap[y * ResX + x] * MeshParams.HeightMultiplier;
			Vertex.Position = Position;

			FColor Color = FColor::Green;
			if (MeshParams.TerrainLayers.Num() > 0)
			{
				//TODO: sorting
				for (const FTerrainLayer& Layer : MeshParams.TerrainLayers)
				{
					if (Position.Z < Layer.MaxHeight)
					{
						Color = Layer.Color;
						break;
					}
				}
			}
			Vertex.Color = Color;

			FVector2D UV0;
			UV0.X = x / ResX;
			UV0.Y = y / ResY;
			Vertex.UV0 = UV0;
			Vertices.Add(FRuntimeMeshVertexSimple{ Position, Color }); //TODO: pixel colors

			if (x < ResX - 1 && y < ResY - 1)
			{
				Triangles.Add(vertexIndex);
				Triangles.Add(vertexIndex + ResX + 1);
				Triangles.Add(vertexIndex + ResX);

				Triangles.Add(vertexIndex + ResX + 1);
				Triangles.Add(vertexIndex);
				Triangles.Add(vertexIndex + 1);
			}

			vertexIndex++;
		}

	if (MeshParams.Material != nullptr)
		RuntimeMesh->SetMaterial(Id, MeshParams.Material);
	RuntimeMesh->CreateMeshSection(Id, Vertices, Triangles, false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::MoveArrays);
}

void UTerrainChunk::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	RuntimeMesh->ClearMeshSection(Id);
}

