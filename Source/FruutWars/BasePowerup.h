// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePowerup.generated.h"

class ABaseCharacter;
class URotatingMovementComponent;
class USphereComponent;
class USceneComponent;

UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API ABasePowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePowerup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USphereComponent* HitBox {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		URotatingMovementComponent* RotatingMovementComponent {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		USceneComponent* PowerUpLocation {};

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ActivatePowerup(ABaseCharacter* character);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
