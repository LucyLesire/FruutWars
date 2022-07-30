// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto locationLeft = FurthestTiles.Key->GetActorLocation();
	auto locationRight = FurthestTiles.Value->GetActorLocation();
	if(locationLeft.Z < SunkValue || locationRight.Z < SunkValue)
	{
		GetFurthestTiles();
	}
}

void ACameraDirector::GetFurthestTiles()
{
	for (auto tile : Tiles)
	{
		auto location = tile->GetActorLocation();
		if(location.Z > SunkValue)
		{
			if (location.X < FurthestTiles.Key->GetActorLocation().X)
			{
				FurthestTiles.Key = tile;
			}
			else if (location.X > FurthestTiles.Value->GetActorLocation().X)
			{
				FurthestTiles.Value = tile;
			}
		}
	}
}

FVector ACameraDirector::GetNewCameraLocation()
{
	return {};
}



