// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class FRUUTWARS_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

	AActor* UsedActor = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector GetLocation() const { return GetActorLocation(); };

	UFUNCTION(BlueprintCallable)
	bool GetOccupied();

	UFUNCTION(BlueprintCallable)
	void SetOccupied(AActor* usedActor);

	UFUNCTION(BlueprintCallable)
	AActor* GetUsedActor() const { return UsedActor; };
};
