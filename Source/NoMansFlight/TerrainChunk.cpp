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
	
	SetWorldLocation(FVector{ ChunkCoord.X * 200.f, ChunkCoord.Y * 200.f, 0.f });

	FName MeshName{ *FString::Printf(TEXT("ChunkMesh_%d_%d"), ChunkCoord.X, ChunkCoord.Y) };
	

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FRuntimeMeshTangent> Tangents;
	TArray<FVector2D> TextureCoordinates;
	TArray<int32> Triangles;

	URuntimeMeshLibrary::CreateBoxMesh(FVector(200, 200, 25), Vertices, Triangles, Normals, TextureCoordinates, Tangents);
	for (auto& Vertex : Vertices)
		Vertex += GetComponentLocation();

	RuntimeMesh->SetRelativeScale3D(MeshScale);
	// Create the mesh section specifying collision
	RuntimeMesh->CreateMeshSection(Id, Vertices, Triangles, Normals, TextureCoordinates, TArray<FColor>(), Tangents, true, EUpdateFrequency::Infrequent);
}

// Called every frame
void UTerrainChunk::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UTerrainChunk::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	RuntimeMesh->ClearMeshSection(Id);
}

