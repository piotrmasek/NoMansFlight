// Fill out your copyright notice in the Description page of Project Settings.

#include "NoMansFlight.h"
#include "TerrainGen.h"
#include "../../Plugins/SimplexNoise/Source/SimplexNoise/Public/SimplexNoiseBPLibrary.h"
#include "TerrainChunk.h"
#include "RuntimeMeshComponent.h"
#include "EngineGlobals.h"

DECLARE_CYCLE_STAT(TEXT("TerrainGen ~ UpdateChunks"), STAT_UpdateChunks, STATGROUP_TerrainGen);
DECLARE_CYCLE_STAT(TEXT("TerrainGen ~ CreateChunk"), STAT_CreateChunk, STATGROUP_TerrainGen);
DECLARE_CYCLE_STAT(TEXT("TerrainGen ~ RemoveChunk"), STAT_RemoveChunk, STATGROUP_TerrainGen);
DECLARE_CYCLE_STAT(TEXT("TerrainGen ~ GenHeightMap"), STAT_GenHeightMap, STATGROUP_TerrainGen);

// Sets default values
ATerrainGen::ATerrainGen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	bUpdateChunks = true;
	ChunkSize = FVector{ 5000.f, 5000.f, 1000.f };
	ChunkVisibilityRange = 3;
	
	
	RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));
}

// Called when the game starts or when spawned
void ATerrainGen::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle DummyHandle;
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([this]() { bUpdateChunks = true; });
	GetWorld()->GetTimerManager().SetTimer(DummyHandle, TimerCallback, 0.5f, true); //TODO: add timer settings n stuff
#if !WITH_EDITOR
	if(GEngine && GEngine->GetNetMode(GWorld) == NM_Standalone)
		this->LoadConfig();
#endif
	//if (Water)
	//{
	//	Water->SetWorldScale3D(FVector(ChunkSize.X / 100.f * ChunkVisibilityRange, 
	//		ChunkSize.Y / 100.f * ChunkVisibilityRange, 1.f)); // /100.f for plane mesh, temp
	//}
		
}

// Called every frame
void ATerrainGen::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (bUpdateChunks)
	{
		UpdateChunks();
	}

}

void ATerrainGen::GenerateHeightMap(TArray<float>& OutHeightMap, int32 SizeX, int32 SizeY, float Scale, int32 Octaves, float Persistance, float Lacunarity, FVector2D Offset, int32 Seed)
{
	SCOPE_CYCLE_COUNTER(STAT_GenHeightMap);
	
	OutHeightMap.Empty(SizeX * SizeY);

	TArray<FVector2D> OctaveOffsets;
	float Amplitude = 1.f;
	float Frequency = 1.f;
	float MaxHeight = 0.f;


	FMath::RandInit(Seed);
	for (int32 i = 0; i < Octaves; ++i)
	{
		float OffsetX = FMath::FRandRange(-10000.f, 10000.f) - Offset.X;
		float OffsetY = FMath::FRandRange(-10000.f, 10000.f) + Offset.Y;
		OctaveOffsets.Add(FVector2D{ OffsetX, OffsetY });

		MaxHeight += Amplitude;
		Amplitude *= Persistance;
	}

	float HalfSizeX = SizeX / 2.f;
	float HalfSizeY = SizeY / 2.f;


	for(int32 y = 0; y < SizeY; ++y)
	{
		for (int32 x = 0; x < SizeX; ++x)
		{

			float NoiseHeight = 0.f;
			Amplitude = 1.f;
			Frequency = 1.f;

			for (const auto& OctaveOffset : OctaveOffsets)
			{
				float SampleX = (x - HalfSizeX + OctaveOffset.X) / Scale * Frequency;
				float SampleY = (y - HalfSizeY + OctaveOffset.Y) / Scale * Frequency;

				float Perlin = USimplexNoiseBPLibrary::SimplexNoiseInRange2D(SampleX, SampleY, -1.f, 1.f);
				NoiseHeight += Perlin * Amplitude;

				Amplitude *= Persistance;
				Frequency *= Lacunarity;
			}

			NoiseHeight = (NoiseHeight + 1.f) / (MaxHeight); //TODO: normalization factor as property?
			NoiseHeight = FMath::Clamp(NoiseHeight, 0.f, FLT_MAX);
			OutHeightMap.Add(NoiseHeight);
		}
	}

}

void ATerrainGen::UpdateChunks()
{
	SCOPE_CYCLE_COUNTER(STAT_UpdateChunks);
	
	FVector PlayerPos = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();//TODO: various playerids?
	//UE_LOG(LogTemp, Log, TEXT("PPos = %s"), *PlayerPos.ToString());
	PlayerPos /= ChunkSize;
	FIntVector PlayerChunkCoord{ (int32)PlayerPos.X, (int32)PlayerPos.Y, (int32)PlayerPos.Z };
	//UE_LOG(LogTemp, Log, TEXT("CCoord = %s"), *PlayerChunkCoord.ToString());
	
	
	for (int i = TerrainChunks.Num() - 1; i >= 0; --i)
	{
		if (FMath::Abs(TerrainChunks[i]->ChunkCoord.X - PlayerChunkCoord.X) > ChunkVisibilityRange || FMath::Abs(TerrainChunks[i]->ChunkCoord.Y - PlayerChunkCoord.Y) > ChunkVisibilityRange)
		{
			RemoveChunk(TerrainChunks[i]);
		}
	}
	
	
	for(int32 y = PlayerChunkCoord.Y - ChunkVisibilityRange; y <= PlayerChunkCoord.Y + ChunkVisibilityRange; ++y)
		for (int32 x = PlayerChunkCoord.X - ChunkVisibilityRange; x <= PlayerChunkCoord.X + ChunkVisibilityRange; ++x)
		{
			if (nullptr == TerrainChunks.FindByPredicate([&](UTerrainChunk* Chunk) {return Chunk->ChunkCoord.X == x && Chunk->ChunkCoord.Y == y; }))
			{
				CreateChunk(FIntVector{ x, y, 0 });
			}
		}




}

void ATerrainGen::CreateChunk(FIntVector ChunkCoord)
{
	SCOPE_CYCLE_COUNTER(STAT_CreateChunk);
	FName ChunkName{ *FString::Printf(TEXT("TerrainChunk_%d_%d"), ChunkCoord.X, ChunkCoord.Y) };

	UTerrainChunk* NewChunk = NewObject<UTerrainChunk>(this, ChunkName);
	NewChunk->ChunkCoord = ChunkCoord;
	NewChunk->ChunkSize = ChunkSize;

	TerrainChunks.Add(NewChunk);
	
	FAttachmentTransformRules TransRules{ EAttachmentRule::KeepWorld, false };
	NewChunk->AttachToComponent(GetRootComponent(), TransRules);
	NewChunk->Init(RuntimeMesh->FirstAvailableMeshSectionIndex());
}

void ATerrainGen::RemoveChunk(UTerrainChunk* ChunkToRemove)
{
	SCOPE_CYCLE_COUNTER(STAT_RemoveChunk);
	TerrainChunks.RemoveSingleSwap(ChunkToRemove, false);
	ChunkToRemove->DestroyComponent();
}

#if WITH_EDITOR
void ATerrainGen::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	this->UpdateDefaultConfigFile();
}
#endif
