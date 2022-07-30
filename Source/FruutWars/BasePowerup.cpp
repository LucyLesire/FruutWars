// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePowerup.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "BaseCharacter.h"

// Sets default values
ABasePowerup::ABasePowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create collision hitbox for powerup
	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
	HitBox->SetSphereRadius(50.f);
	SetRootComponent(HitBox);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotation"));

	//Attach hitbox to location of the powerup
	PowerUpLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Powerup Location"));
	PowerUpLocation->SetupAttachment(HitBox);

}

// Called when the game starts or when spawned
void ABasePowerup::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ABasePowerup::PlayerBeginOverlap);

	PowerUpLocation->AttachTo(RootComponent);
}

// Called every frame
void ABasePowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasePowerup::PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//On overlap with player activate the powerup
	if(ABaseCharacter * character{Cast<ABaseCharacter>(OtherActor)})
	{
		ActivatePowerup(character);
		Destroy();
	}
}
