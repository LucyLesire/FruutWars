// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class ABaseCharacter;
class UProjectileMovementComponent;
class UStaticMeshComponent;
class ABaseWeapon;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
		USphereComponent* HitBox{};

	/** The mesh of the projectile, can be left empty.*/
	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* ProjectileMesh{};

	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		USphereComponent* ExplosionHitBox{};
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Sets the initial values of the projectile*/
	void InitializeProjectile(ABaseCharacter* PlayerOwner, ABaseWeapon* Weapon, const FVector& Velocity, float DamageMultiplier = 1.f);

	/** Explodes the projectile, but does not destroy it.*/
	UFUNCTION(BlueprintCallable)
		void Explode();

protected:

	/** Projectile movement component that will control the movement of the projectile and the bounces.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UProjectileMovementComponent* ProjectileComponent {};

	/** When the projectile hits a player that is not its owner. the default implementation will destroy the projectile and damage the player.*/
	UFUNCTION(BlueprintNativeEvent)
		void OnProjectileHitCharacter(ABaseCharacter* target);

	/** When the projectile hits the owning player.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnProjectileHitTeam(ABaseCharacter* target);

	/** Fires when the projectile is created.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnProjectileCreate();

	/** Fires when the projectile runs out of time and despawns.*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnProjectileDespawns();

	/** Fires when the projectile gets exploded*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnProjectileExplode(float radius);

	UFUNCTION(BlueprintNativeEvent)
	void ProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UFUNCTION()
	/** Function that will be bound to the projectile bounce */
	void ProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

protected:

	/**
	 * VARIABLES
	 */

	/** Defines how many bounces the projectile can still do. this gets decremented each time it bounces.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
		int Bounces{ 0 };

	/** The base damage of the projectile.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
		float BaseDamage{ 10.f };

	/** The base damage multiplied with the damage multiplier of the weapon.*/
	UPROPERTY(BlueprintReadWrite, Category = Damage)
		float Damage{};

	/** Keeps the time the projectile has been alive.*/
	UPROPERTY(BlueprintReadOnly, Category = Time)
		float TimeAlive{};

	/** Time it takes to despawn (can be -1 so it doesn't despawn).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Time)
		float DespawnTime{ -1.f };

	/** Reference to the player that shot the projectile.*/
	UPROPERTY(BlueprintReadOnly, Category = Owner)
		ABaseCharacter* OwningPlayer{};

	/** Reference to the weapon that shot the projectile.*/
	UPROPERTY(BlueprintReadOnly, Category = Owner)
		ABaseWeapon* OwningWeapon{};

	/** The radius of the explosion (can be 0).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damage)
		float ExplosionRadius{};

	/** If true will damage the player/teammate that shot the projectile.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damage)
		bool bFriendlyFire{ true };

	/** Only use the despawn time if the projectile has zero bounces left.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	bool bBounceDespawn{};

};

