// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDrop.generated.h"

class USphereComponent;
class URotatingMovementComponent;
class ABaseWeapon;
class USceneComponent;

UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API AWeaponDrop : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USphereComponent* HitBox {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		URotatingMovementComponent* RotatingMovementComponent {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		USceneComponent* WeaponLocation {};


public:	
	// Sets default values for this actor's properties
	AWeaponDrop();

	UFUNCTION(BlueprintCallable)
	void SetWeapon(ABaseWeapon* Weapon);

	virtual void BeginPlay() override;

	UFUNCTION()
		void PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:

	ABaseWeapon* CurrentWeapon{};

	
};


