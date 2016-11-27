// Fill out your copyright notice in the Description page of Project Settings.

#include "NoMansFlight.h"
#include "TerrainGen.h"
#include "../../Plugins/SimplexNoise/Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"
#include "RuntimeMeshComponent.h"

// Sets default values
ATerrainGen::ATerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	ChunkSize = FVector{ 5000.f, 5000.f, 1000.f };
	ChunkVisibilityRange = 3;

	ResX = ResY = 200;
	Scale = 25.f;
	Octaves = 4;
	Persistence = 0.5f;
	Lacunarity = 2;
	Seed = FMath::Rand();
	HeightMultiplier = 25.f;
	
	
	RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));
}

// Called when the game starts or when spawned
void ATerrainGen::BeginPlay()
{
	Super::BeginPlay();

	/*TArray<float> HeightMap = GenerateHeightMap(ResX, ResY, Scale, Octaves, Persistence, Lacunarity, Offset, Seed);
	CreateMesh(HeightMap);*/
	//for (int i = 0; i < 9; i++)
	//{
	//	CreateChunk(FIntVector{ i });
	//}
	
	FTimerHandle DummyHandle;
	GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ATerrainGen::UpdateChunks, 1.f, true); //TODO: add timer settings n stuff
}

// Called every frame
void ATerrainGen::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	

}

TArray<float> ATerrainGen::GenerateHeightMap(int32 SizeX, int32 SizeY, float Scale, int32 Octaves, float Persistance, float Lacunarity, FVector2D Offset, int32 Seed)
{
	
	TArray<float> HeightMap;
	HeightMap.SetNumUninitialized(SizeX * SizeY);

	TArray<FVector2D> OctaveOffsets;
	
	FMath::RandInit(Seed);
	for (int32 i = 0; i < Octaves; ++i)
	{
		float OffsetX = FMath::FRandRange(-10000.f, 10000.f) + Offset.X;
		float OffsetY = FMath::FRandRange(-10000.f, 10000.f) + Offset.Y;
		OctaveOffsets.Add(FVector2D{ OffsetX, OffsetY });
	}

	float MaxNoiseHeight = MIN_flt;
	float MinNoiseHeight = MAX_flt;

	float HalfSizeX = SizeX / 2.f;
	float HalfSizeY = SizeY / 2.f;


	for(int32 y = 0; y < SizeY; ++y)
	{
		for (int32 x = 0; x < SizeX; ++x)
		{
			float Amplitude = 1.f;
			float Frequency = 1.f;
			float NoiseHeight = 0.f;
			
			for (const auto& OctaveOffset : OctaveOffsets)
			{
				float SampleX = (x - HalfSizeX) / Scale * Frequency + OctaveOffset.X;
				float SampleY = (y - HalfSizeY) / Scale * Frequency + OctaveOffset.Y;

				float Perlin = USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -1.f, 1.f);
				NoiseHeight += Perlin * Amplitude;

				Amplitude *= Persistance;
				Frequency *= Lacunarity;
			}

			if (NoiseHeight > MaxNoiseHeight)
			{
				MaxNoiseHeight = NoiseHeight;
			}
			else if (NoiseHeight < MinNoiseHeight)
			{
				MinNoiseHeight = NoiseHeight;
			}

			HeightMap[y * SizeX + x] = NoiseHeight;
		}
	}

	for (float& NoiseHeight : HeightMap)
	{
		NoiseHeight = (NoiseHeight - MinNoiseHeight) / (MaxNoiseHeight - MinNoiseHeight);
	}

	return HeightMap;
}

void ATerrainGen::CreateMesh(const TArray<float>& HeightMap)
{	
	TArray<FRuntimeMeshVertexSimple> Vertices;
	TArray<int32> Triangles;

	FVector MeshScale = ChunkSize;
	MeshScale.X /= ResX;
	MeshScale.Y /= ResY;
	MeshScale.Z /= HeightMultiplier;

	RuntimeMesh->SetRelativeScale3D(MeshScale);

	float TopLeftX = (ResX - 1) / 2.f;
	float TopLeftY = (ResY - 1) / -2.f;

	int vertexIndex = 0;
	for (int y = 0; y < ResY; ++y)
		for (int x = 0; x < ResX; ++x)
		{
			FRuntimeMeshVertexSimple Vertex;
			
			FVector Position;
			Position.X = TopLeftX - x;
			Position.Y = TopLeftY + y;
			Position.Z = HeightMap[y * ResX + x] * HeightMultiplier;
			Vertex.Position = Position;

			FColor Color = FColor::Green;
			if (TerrainLayers.Num() > 0)
			{
				//TODO: sorting
				for (const FTerrainLayer& Layer : TerrainLayers)
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

	if (Material != nullptr)
		RuntimeMesh->SetMaterial(0, Material);
	RuntimeMesh->CreateMeshSection(0, Vertices, Triangles);
	
}

void ATerrainGen::UpdateChunks()
{
	FVector PlayerPos = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();//TODO: various playerids?
	UE_LOG(LogTemp, Log, TEXT("PPos = %s"), *PlayerPos.ToString());
	PlayerPos /= ChunkSize;
	FIntVector PlayerChunkCoord{ (int32)PlayerPos.X, (int32)PlayerPos.Y, (int32)PlayerPos.Z };
	UE_LOG(LogTemp, Log, TEXT("CCoord = %s"), *PlayerChunkCoord.ToString());
	for(int32 y = PlayerChunkCoord.Y - ChunkVisibilityRange; y <= PlayerChunkCoord.Y + ChunkVisibilityRange; ++y)
		for (int32 x = PlayerChunkCoord.X - ChunkVisibilityRange; x <= PlayerChunkCoord.X + ChunkVisibilityRange; ++x)
		{
			if (nullptr == TerrainChunks.FindByPredicate([&](UTerrainChunk* Chunk) {return Chunk->ChunkCoord.X == x && Chunk->ChunkCoord.Y == y; }))
			{
				CreateChunk(FIntVector{ x, y, 0 });
			}
		}

	for (auto& Chunk : TerrainChunks)
	{
		if (FMath::Abs(Chunk->ChunkCoord.X - PlayerChunkCoord.X) > ChunkVisibilityRange || FMath::Abs(Chunk->ChunkCoord.Y - PlayerChunkCoord.Y) > ChunkVisibilityRange)
		{
			RemoveChunk(Chunk);
		}
	}


}

void ATerrainGen::CreateChunk(FIntVector ChunkCoord)
{
	FName ChunkName{ *FString::Printf(TEXT("TerrainChunk_%d_%d"), ChunkCoord.X, ChunkCoord.Y) };

	UTerrainChunk* NewChunk = NewObject<UTerrainChunk>(this, ChunkName);
	NewChunk->ChunkCoord = ChunkCoord;

	FVector MeshScale = ChunkSize;
	MeshScale.X /= ResX;
	MeshScale.Y /= ResY;
	MeshScale.Z /= HeightMultiplier;
	NewChunk->MeshScale = MeshScale;

	TerrainChunks.Add(NewChunk);
	
	FAttachmentTransformRules TransRules{ EAttachmentRule::KeepWorld, false };
	NewChunk->AttachToComponent(GetRootComponent(), TransRules);
	NewChunk->Init(RuntimeMesh->FirstAvailableMeshSectionIndex(0));
}

void ATerrainGen::RemoveChunk(UTerrainChunk* ChunkToRemove)
{
	TerrainChunks.Remove(ChunkToRemove);
	ChunkToRemove->DestroyComponent();
}


