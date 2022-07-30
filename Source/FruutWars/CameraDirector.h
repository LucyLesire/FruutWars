// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class FRUUTWARS_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category= "Camera")
		AActor* Camera;

	TPair<AActor*, AActor*> FurthestTiles;

	UPROPERTY(EditDefaultsOnly, Category = "Tiles")
	TArray<AActor*> Tiles;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		float MovementSpeed;

	UFUNCTION(BlueprintImplementableEvent)
		void InitializeTiles();

	UPROPERTY(EditDefaultsOnly, Category = "Tile")
		float SunkValue;

	UPROPERTY(EditDefaultsOnly, Category = "Tile")
		float TileOffset;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void GetFurthestTiles();
	FVector GetNewCameraLocation();
};
