// Fill out your copyright notice in the Description page of Project Settings.

#include "NoMansFlight.h"
#include "TerrainChunk.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshLibrary.h"
#include "TerrainGen.h"

DECLARE_CYCLE_STAT(TEXT("TerrainChunk ~ GenerateMesh"), STAT_GenerateMesh, STATGROUP_TerrainGen);
DECLARE_CYCLE_STAT(TEXT("TerrainChunk ~ Init"), STAT_ChunkInit, STATGROUP_TerrainGen);
DECLARE_CYCLE_STAT(TEXT("TerrainChunk ~ OnDestroyed"), STAT_OnDestroyed, STATGROUP_TerrainGen);

// Sets default values for this component's properties
UTerrainChunk::UTerrainChunk()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;
	
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
	SCOPE_CYCLE_COUNTER(STAT_ChunkInit);
	
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
	

	FVector Location{ ChunkCoord.X * ChunkSize.X, ChunkCoord.Y * ChunkSize.Y, 0.f };

	Location.X -= FMath::Sign(Location.X) * ChunkSize.X / NoiseParams.ResX;
	Location.Y -= FMath::Sign(Location.Y) * ChunkSize.Y / NoiseParams.ResY;

	SetWorldLocation(Location);

	
	
	ATerrainGen::GenerateHeightMap(HeightMap,
		NoiseParams.ResX + 2,
		NoiseParams.ResY + 2,
		NoiseParams.Scale,
		NoiseParams.Octaves,
		NoiseParams.Persistence,
		NoiseParams.Lacunarity,
		FVector2D{ static_cast<float>(ChunkCoord.X * NoiseParams.ResX), static_cast<float>(ChunkCoord.Y * NoiseParams.ResY) },
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
	SCOPE_CYCLE_COUNTER(STAT_GenerateMesh);
	
	TArray<FRuntimeMeshVertexSimple> Vertices;
	TArray<int32> Triangles;
	int32 ResX = NoiseParams.ResX;
	int32 ResY = NoiseParams.ResY;

	Vertices.Reserve((ResX) * (ResY));

	FVector MeshScale = ChunkSize;
	MeshScale.X /= ResX;
	MeshScale.Y /= ResY;
	MeshScale.Z /= MeshParams.HeightMultiplier;

	RuntimeMesh->SetRelativeScale3D(MeshScale);

	float TopLeftX = (ResX) / 2.f + ChunkCoord.X * (ResX);
	float TopLeftY = (ResY) / -2.f + ChunkCoord.Y * (ResY);

	int vertexIndex = 0;
	for (int y = 0; y < ResY + 2; ++y)
		for (int x = 0; x < ResX + 2; ++x)
		{
			FRuntimeMeshVertexSimple Vertex;

			FVector Position;
			Position.X = TopLeftX - x;
			Position.Y = TopLeftY + y;
			Position.Z = HeightMap[y * (ResX + 2) + x] * MeshParams.HeightMultiplier;
			Vertex.Position = Position;

			FColor Color = FColor::Green;
			if (MeshParams.TerrainLayers.Num() > 0)
			{
				//TODO: sorting
				for (const FTerrainLayer& Layer : MeshParams.TerrainLayers)
				{
					Color = Layer.Color;
					if (Position.Z < Layer.MaxHeight)
					{				
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

			if (x < ResX && y < ResY)
			{
				Triangles.Add(vertexIndex);
				Triangles.Add(vertexIndex + ResX + 2 + 1);
				Triangles.Add(vertexIndex + ResX + 2);

				Triangles.Add(vertexIndex + ResX + 2 + 1);
				Triangles.Add(vertexIndex);
				Triangles.Add(vertexIndex + 1);
			}

			vertexIndex++;
		}

	//URuntimeMeshLibrary::CreateGridMeshTriangles(ResX, ResY, false, Triangles);

	if (MeshParams.Material != nullptr)
		RuntimeMesh->SetMaterial(Id, MeshParams.Material);
	RuntimeMesh->CreateMeshSection(Id, Vertices, Triangles, false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::MoveArrays);
}

void UTerrainChunk::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	SCOPE_CYCLE_COUNTER(STAT_OnDestroyed);
	RuntimeMesh->ClearMeshSection(Id);
}

