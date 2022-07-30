// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "BaseProjectile.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // no collision on the weapon mesh
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetRenderCustomDepth(true);

	WeaponMuzzle = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponMuzzle"));
	WeaponMuzzle->SetupAttachment(WeaponMesh);

	DamageMultiplier = BaseDamageMultiplier;
	MultiplierTime = MaxMultiplierTime;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	AActor::BeginPlay();
	if (WeaponName == EWeaponIcons::VE_Weapon_Banana || WeaponName == EWeaponIcons::VE_Weapon_Coconut || WeaponName == EWeaponIcons::VE_Weapon_GrenadeApple || WeaponName == EWeaponIcons::VE_Weapon_Lemon)
		bHasThrowable = true;
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	// cooldown on weapon
	if (bIsRecharging) 
	{
		RechargeTime -= DeltaTime;
		if (RechargeTime <= 0)
		{
			bIsRecharging = false;
			OnWeaponRecharged();
		}
	}

	const float offset = 0.01f;
	if(DamageMultiplier < (MaxMultiplierTime - offset) || DamageMultiplier > (MaxMultiplierTime + offset))
	{
		MultiplierTime -= DeltaTime;
		if(MultiplierTime <= 0)
		{
			DamageMultiplier = BaseDamageMultiplier;
			MultiplierTime = MaxMultiplierTime;
		}
	}
}

void ABaseWeapon::Fire(const FVector& Direction)
{
	if (!OwningCharacter) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Weapon doesnt have an owning character"));


	if (!bIsRecharging && ProjectileClass)
	{
		// Make multiple bullets if projectiles fires is larger than 1
		for (int i{}; i < ProjectilesFired; ++i)
		{
			// Calculate random bullet spread
			float randomBulletSpread = (BulletSpread > 0.f) ? FMath::FRandRange(-BulletSpread, BulletSpread) : 0.f;
			FVector newDirection = Direction.RotateAngleAxis(randomBulletSpread, { 0,0,1 });

			// Calculate random bullet speed
			float randomBulletSpeed = (SpeedDeviation > 0.f) ? FMath::FRandRange(-SpeedDeviation, SpeedDeviation) : 0.f;
			float newSpeed = ProjectileSpeed + randomBulletSpeed;

			// Spawn the projectile
			FVector location = WeaponMuzzle->GetComponentLocation();
			FRotator rotation = newDirection.Rotation();

			FActorSpawnParameters spawnParam{};
			spawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* Projectile = GetWorld()->SpawnActor(ProjectileClass, &location, &rotation, spawnParam);

			if (!OwningCharacter->GetController()) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, TEXT("Character doesn't have a controller"));

			OnShoot.Broadcast();

			if (auto ProjectileCast{Cast<ABaseProjectile>(Projectile)}) {
				// Set the necessary value
				ProjectileCast->InitializeProjectile(OwningCharacter, this, newDirection * newSpeed, DamageMultiplier);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Projectile wasn't initialized correctly"));
			}
		}

		OnWeaponFire(Direction);

		if (--Ammo == 0) // if ammo starts as negative it will never go in this if statement (intended)
		{
			// todo magic number setting secondary (because primary weapons have infinite ammo)
			OnWeaponNoAmmo();
			OwningCharacter->SetSecondaryWeapon(nullptr);
			if (OwningCharacter && OwningCharacter->GetSecondaryWeapon() == this)
				OwningCharacter->SetSecondaryWeapon(nullptr);
			Destroy();
		}
		RechargeTime = FireCooldown;
		bIsRecharging = true;
	}
}

void ABaseWeapon::SetDamageMultiplier(float amount)
{
	if(amount > 0.f)
		DamageMultiplier = amount;
}


