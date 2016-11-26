// Fill out your copyright notice in the Description page of Project Settings.

#include "NoMansFlight.h"
#include "TerrainChunk.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshLibrary.h"


// Sets default values for this component's properties
UTerrainChunk::UTerrainChunk()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;


	// ...

	//RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));

}


// Called when the game starts
void UTerrainChunk::InitializeComponent()
{
	Super::InitializeComponent();
	
	
	// ...
}


void UTerrainChunk::Init()
{
	SetWorldLocation(FVector{ ChunkCoord.X * 200.f, ChunkCoord.Y * 200.f, 0.f });

	FName MeshName{ *FString::Printf(TEXT("ChunkMesh_%d_%d"), ChunkCoord.X, ChunkCoord.Y) };
	RuntimeMesh = NewObject<URuntimeMeshComponent>(this, MeshName);
	FAttachmentTransformRules TransformRules{ EAttachmentRule::SnapToTarget, false };
	RuntimeMesh->AttachToComponent(this, TransformRules);

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FRuntimeMeshTangent> Tangents;
	TArray<FVector2D> TextureCoordinates;
	TArray<int32> Triangles;

	URuntimeMeshLibrary::CreateBoxMesh(FVector(200, 200, 25), Vertices, Triangles, Normals, TextureCoordinates, Tangents);


	RuntimeMesh->SetRelativeScale3D(MeshScale);
	// Create the mesh section specifying collision
	RuntimeMesh->CreateMeshSection(0, Vertices, Triangles, Normals, TextureCoordinates, TArray<FColor>(), Tangents, true, EUpdateFrequency::Infrequent);
}

// Called every frame
void UTerrainChunk::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

