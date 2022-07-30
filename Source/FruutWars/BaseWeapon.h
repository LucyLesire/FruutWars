// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class ABaseProjectile;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EWeaponIcons : uint8{
	VE_Weapon_GrapeGun	UMETA(DisplayName = "GrapeGun"),
	VE_Weapon_CornShotgun	UMETA(DisplayName = "CornShotgun"),
	VE_Weapon_Banana	UMETA(DisplayName = "Banana"),
	VE_Weapon_Coconut	UMETA(DisplayName = "Coconut"),
	VE_Weapon_GrenadeApple	UMETA(DisplayName = "GrenadeApple"),
	VE_Weapon_Lemon	UMETA(DisplayName = "Lemon"),
	VE_Weapon_None	UMETA(DisplayName = "None"),
};


UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* WeaponMesh {};

public:
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	/** The location of where the projectiles should be fired from.*/
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* WeaponMuzzle {};

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Fires the weapon in the given direction*/
	UFUNCTION(BlueprintCallable)
		void Fire(const FVector& Direction);

	UFUNCTION(BlueprintCallable)
		void SetDamageMultiplier(float amount);


protected:

	/** Activates when a projectile is fired*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponFire(const FVector& direction);

	/**
	 * Activates when the weapon is able to fire again.
	 * Every time the weapon is fired there is an amount of time that has to pas (set with fireRate) before the weapon can fire again
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponRecharged();

	/** Activates when the weapon has no more ammo and will be disabled*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponNoAmmo();

	/** Activated when the weapon is picked up by a player*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponPickedUp();


public:

	/** Returns true if the weapon is not able to fire*/
	UFUNCTION(BlueprintCallable)
		bool IsRecharging() { return bIsRecharging; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShootSignature);
	UPROPERTY(BlueprintAssignable)
		FOnShootSignature OnShoot;

protected:

	/**
	 * VARIABLES
	 */

	 /** This defines the intervals of how fast the weapon is able to fire projectiles.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float FireCooldown{ 0.5f };

	/** This value determines the current damage multiplier for the damage of the projectile*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shooting)
		float DamageMultiplier{ };

	/** This value determines the base damage multiplier for the damage of the projectile*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float BaseDamageMultiplier{ 1.f };

	/** This value determines the damage multiplier for the damage of the projectile*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shooting)
		float MultiplierTime{};

	/** This value determines the damage multiplier for the damage of the projectile*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float MaxMultiplierTime{5.f};


	/** Defines how many projectiles are fired.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		int ProjectilesFired{ 1 };

	/** Determines the velocity of the fired projectiles.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float ProjectileSpeed{ 500.f };

	/** Angle that determines maximum bullet deviation (in Degrees).*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float BulletSpread{ 0.f };

	/** Projectiles may be fired faster/slower depending on this variable.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		float SpeedDeviation{ 0.f };

	/** Name for the weapon (to be displayed in the HUD)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		EWeaponIcons WeaponName{EWeaponIcons::VE_Weapon_None};


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Shooting)
		bool bHasThrowable{ false };

	/**
	 * The amount of times this weapon can be shot (goes down after every shot).
	 * Use -1 to for infinite ammo.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Shooting)
		int Ammo{ -1 };

	bool bIsRecharging;

	float RechargeTime;

	/** A reference to the owning character*/
	UPROPERTY(BlueprintReadOnly)
		ABaseCharacter* OwningCharacter{};

	/** The class the the projectile that will be fired.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<ABaseProjectile> ProjectileClass{};

public:

	inline void SetOwnerCharacter(ABaseCharacter* character) { OwningCharacter = character; }
	inline UStaticMeshComponent* GetWeaponMesh() { return WeaponMesh; }

};
