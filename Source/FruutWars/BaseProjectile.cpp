// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Hit Box"));
	HitBox->SetSphereRadius(25.f);
	HitBox->SetCollisionProfileName(TEXT("Projectile"));
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(HitBox);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(HitBox);
	ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Set The default projectile component variables. These can also be accessed through the blueprints editor
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileComponent->ProjectileGravityScale = 0.f;
	ProjectileComponent->bRotationFollowsVelocity = true;
	ProjectileComponent->bShouldBounce = true;
	ProjectileComponent->Bounciness = 1.f;
	ProjectileComponent->Friction = 0.f;
	ProjectileComponent->BounceVelocityStopSimulatingThreshold = 0.f;
	ProjectileComponent->UpdatedComponent = HitBox;

	ExplosionHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("Explosion Hit Box"));
	ExplosionHitBox->SetupAttachment(HitBox);
	ExplosionHitBox->SetSphereRadius(ExplosionRadius);
	ExplosionHitBox->SetCollisionProfileName(TEXT("Projectile"));
	ExplosionHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	AActor::BeginPlay();

	ExplosionHitBox->SetSphereRadius(ExplosionRadius);

	ProjectileComponent->OnProjectileBounce.AddDynamic(this, &ABaseProjectile::ProjectileBounced);

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::ProjectileBeginOverlap);
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	TimeAlive += DeltaTime;

	if (DespawnTime != -1.f && !(bBounceDespawn && Bounces == 0)) {
		DespawnTime -= DeltaTime;
		if (DespawnTime <= 0.f) Destroy();
	}
}

void ABaseProjectile::InitializeProjectile(ABaseCharacter* PlayerOwner, ABaseWeapon* Weapon, const FVector& Velocity,
	float DamageMultiplier)
{
	Damage = DamageMultiplier * BaseDamage;
	OwningPlayer = PlayerOwner;
	OwningWeapon = Weapon;
	ProjectileComponent->Velocity = Velocity;
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABaseProjectile::Explode()
{
	OnProjectileExplode(ExplosionRadius);

	TArray<AActor*> actors{};
	ExplosionHitBox->GetOverlappingActors(actors);

	for (AActor* actor : actors)
	{
		if (auto comp = actor->GetComponentByClass(UHealthComponent::StaticClass()))
		{
			if (auto health{ Cast<UHealthComponent>(comp) })
			{
				health->DoDamage(Damage);
			}
		}
	}
}

void ABaseProjectile::ProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (Bounces-- == 0)
	{
		Destroy();
	}
}

void ABaseProjectile::OnProjectileHitCharacter_Implementation(ABaseCharacter* target)
{
	if (OwningPlayer)
		target->HealthComponent->DoDamage(Damage, OwningPlayer->GetController());
	else
		target->HealthComponent->DoDamage(Damage);
	Destroy();
}

void ABaseProjectile::ProjectileBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		if (otherCharacter != OwningPlayer)
			OnProjectileHitCharacter(otherCharacter);
		else 
			OnProjectileHitTeam(otherCharacter);
	}
	else if (auto health{OtherActor->FindComponentByClass<UHealthComponent>()})
	{
		health->DoDamage(Damage);
		Destroy();
	}

}