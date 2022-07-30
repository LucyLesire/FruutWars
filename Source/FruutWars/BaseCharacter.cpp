// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "BaseWeapon.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponent.h"
#include "BaseGameMode.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set the health to the max health

	PrimaryWeaponLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Primary Weapon Location"));
	PrimaryWeaponLocation->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	SecondaryWeaponLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Secondary Weapon Location"));
	SecondaryWeaponLocation->SetupAttachment(GetMesh(), TEXT("hand_lSocket"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	OneDashAmount = MaxEnergyAmount / AmountOfDashes;
	CurrentEnergyAmount = MaxEnergyAmount;

	TimeFreeDash = MaxTimeFreeDash;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::OnCharacterDeath);

	GameMode = Cast<AFWBaseGameMode>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Aiming logic
	if (abs(AimUp) + abs(AimRight) > 0.1f)
	{
		AimDirection.X = AimUp;
		AimDirection.Y = AimRight;
		AimDirection.Normalize();
	}
	else if (GetMovementComponent()->Velocity.SizeSquared() > 1.f)
	{
		FVector movementDirection = GetMovementComponent()->Velocity;
		movementDirection.Z = 0;
		AimDirection = movementDirection.GetSafeNormal();
	}

	// Set player direction towards the aiming
	SetActorRotation(AimDirection.Rotation());
	
	// Dash logic
	if (bDashing)
	{
		DashTimer -= DeltaTime;
		if (DashTimer > 0.f)
		{
			float ZDirection{ GetCharacterMovement()->Velocity.Z };
			GetCharacterMovement()->Velocity = DashSpeed * DashDirection;
			GetCharacterMovement()->Velocity.Z = ZDirection;
		}
		else
		{
			GetCharacterMovement()->Velocity.X = 0.f;
			GetCharacterMovement()->Velocity.Y = 0.f;
			bDashing = false;
		}
	}
	else if (!bCanDash)
	{
		DashRechargeTimer -= DeltaTime;
		if (CurrentEnergyAmount > OneDashAmount)
		{
			if (DashRechargeTimer <= 0)
			{
				bCanDash = true;
			}
		}
	}

	if(CurrentEnergyAmount < MaxEnergyAmount)
	{
		CurrentEnergyAmount += DeltaTime * EnergyReplenish;
		CurrentEnergyAmount = FMath::Clamp(CurrentEnergyAmount, 0.f, MaxEnergyAmount);
	}

	if(bFreeDash)
	{
		TimeFreeDash -= DeltaTime;
		if(TimeFreeDash <= 0)
		{
			TimeFreeDash = MaxTimeFreeDash;
			bFreeDash = false;
		}
	}


}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void ABaseCharacter::Dash()
{
	if (bCanDash)
	{
		auto movement = GetCharacterMovement();

		// If the horizontal movement is almost nothing we take the aim direction for the dash
		if (abs(movement->Velocity.X) + abs(movement->Velocity.Y) > 0.1f)
		{
			DashDirection = FVector(movement->Velocity.X, movement->Velocity.Y, 0);
			DashDirection.Normalize();
		}
		// If the Aim direction is not zero we dash in that direction
		else if (abs(AimDirection.X) + abs(AimDirection.Y) > 0.1f)
		{
			DashDirection = FVector(AimDirection.X, AimDirection.Y, 0);
			DashDirection.Normalize();
		}
		// If there is no suitable direction to dash into we don't dash
		else
		{
			return;
		}

		float ZDirection{ movement->Velocity.Z };
		movement->Velocity = DashDirection * DashSpeed;
		movement->Velocity.Z = ZDirection; // don't change the z direction.

		if (!bFreeDash)
			CurrentEnergyAmount -= OneDashAmount;
		else
			bFreeDash = false;

		bCanDash = false;
		bDashing = true;
		DashTimer = DashTime;
	}
}

void ABaseCharacter::PrimaryFire()
{
	if (PrimaryWeapon && !(SecondaryWeapon && SecondaryWeapon->IsRecharging()))
	{
		FVector direction = GetMesh()->GetRightVector();

		// Aim control
		if (GameMode)
		{
			if (auto player = GameMode->GetPlayerInDirection(PrimaryWeapon->WeaponMuzzle->GetComponentLocation(), direction, 0.05f, GetController<APlayerController>()))
			{
				direction = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			}
		}

		PrimaryWeapon->Fire(direction);
	}
}

void ABaseCharacter::SecondaryFire()
{
	if (SecondaryWeapon && !(PrimaryWeapon && PrimaryWeapon->IsRecharging()))
	{
		FVector direction = GetMesh()->GetRightVector();

		// Aim control
		if (GameMode)
		{
			if (auto player = GameMode->GetPlayerInDirection(GetActorLocation(), direction, 0.05f, GetController<APlayerController>()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Aim assisted"));
				direction = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			}
		}

		SecondaryWeapon->Fire(direction);
	}
}

void ABaseCharacter::SetPrimaryWeapon(ABaseWeapon* weapon)
{
	if (weapon)
	{
		if (PrimaryWeapon) PrimaryWeapon->Destroy();

		PrimaryWeapon = weapon;
		PrimaryWeapon->SetActorTransform(PrimaryWeaponLocation->GetComponentTransform());
		PrimaryWeapon->AttachToComponent(PrimaryWeaponLocation, FAttachmentTransformRules::SnapToTargetIncludingScale);
		PrimaryWeapon->SetOwnerCharacter(this);
		PrimaryWeapon->GetWeaponMesh()->SetCustomDepthStencilValue(GetMesh()->CustomDepthStencilValue);
	}
	else
		PrimaryWeapon = nullptr;
}

void ABaseCharacter::SetSecondaryWeapon(ABaseWeapon* weapon)
{
	if (weapon)
	{
		if (SecondaryWeapon) SecondaryWeapon->Destroy();

		SecondaryWeapon = weapon;
		SecondaryWeapon->SetActorTransform(SecondaryWeaponLocation->GetComponentTransform());
		SecondaryWeapon->AttachToComponent(SecondaryWeaponLocation, FAttachmentTransformRules::SnapToTargetIncludingScale);
		SecondaryWeapon->SetOwnerCharacter(this);
	}
	else
		SecondaryWeapon = nullptr;
}

void ABaseCharacter::ReplenishEnergy()
{
	CurrentEnergyAmount += OneDashAmount;
	CurrentEnergyAmount = FMath::Clamp(CurrentEnergyAmount, 0.f, MaxEnergyAmount);

	bFreeDash = true;
}

float ABaseCharacter::GetCurrentEnergyPercentage()
{
	return (CurrentEnergyAmount / MaxEnergyAmount);
}



