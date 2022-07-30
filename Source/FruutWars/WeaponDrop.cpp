// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDrop.h"

#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "BaseCharacter.h"


// Sets default values
AWeaponDrop::AWeaponDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("HitBox"));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitBox->SetGenerateOverlapEvents(true);
	HitBox->SetSphereRadius(55.f);
	SetRootComponent(HitBox);
	
	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotation"));

	WeaponLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Location"));
	WeaponLocation->SetupAttachment(HitBox);
	WeaponLocation->SetRelativeLocation({ 0,0,200 });
	WeaponLocation->SetRelativeScale3D({ 5,5,5 });
}

void AWeaponDrop::BeginPlay()
{
	Super::BeginPlay();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeaponDrop::PlayerBeginOverlap);

	WeaponLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void AWeaponDrop::PlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Begin Overlap"));
	if (ABaseCharacter * character{ Cast<ABaseCharacter>(OtherActor) })
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Begin Overlap with player"));

		// Transfer ownership to the character
		character->SetSecondaryWeapon(CurrentWeapon);
		CurrentWeapon = nullptr;
		Destroy();
	}
}

void AWeaponDrop::SetWeapon(ABaseWeapon* Weapon)
{
	if (Weapon)
	{
		if (CurrentWeapon) CurrentWeapon->Destroy();
		
		//Weapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Weapon->SetActorTransform(WeaponLocation->GetComponentTransform());
		Weapon->AttachToComponent(WeaponLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//Weapon->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		CurrentWeapon = Weapon;
	}
}


