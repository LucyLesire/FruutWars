// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class ABaseWeapon;
class USceneComponent;
class UHealthComponent;
class AFWBaseGameMode;

UCLASS(Blueprintable, BlueprintType)
class FRUUTWARS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UPROPERTY(EditDefaultsOnly)
		USceneComponent* PrimaryWeaponLocation {};

	UPROPERTY(EditDefaultsOnly)
		USceneComponent* SecondaryWeaponLocation {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UHealthComponent* HealthComponent {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void Dash();

	UFUNCTION(BlueprintCallable)
		void PrimaryFire();

	UFUNCTION(BlueprintCallable)
		void SecondaryFire();

	UFUNCTION(BlueprintCallable)
		void SetPrimaryWeapon(ABaseWeapon* weapon);

	UFUNCTION(BlueprintCallable)
		void SetSecondaryWeapon(ABaseWeapon* weapon);

	UFUNCTION(BlueprintCallable)
		void ReplenishEnergy();

	UFUNCTION(BlueprintCallable)
		float GetCurrentEnergyPercentage();

protected:

	UFUNCTION(BlueprintImplementableEvent)
		void OnCharacterDeath();

protected:

	/**
	 * VARIABLES
	 */

	/** The direction the character is looking/aiming towards*/
	UPROPERTY(BlueprintReadWrite)
		FVector AimDirection {};

	/** The speed of the Dash*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Dash)
	float DashSpeed{ 2000.f };

	/** The time it takes for the dash to finish*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Dash)
		float DashTime{ 0.2f };

	/** The direction of the dash*/
	FVector DashDirection{};

	/** Counts the time for the dash*/
	float DashTimer{};

	/** The time it takes for the dash to become active again*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Dash)
		float DashDelay{};

	/** Counts the time for the delay*/
	float DashRechargeTimer{};

	/** Boolean to see if the player is able to dash*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		bool bCanDash{ true };

	/** Boolean to see if the player is dashing*/
	UPROPERTY(BlueprintReadOnly, Category=Dash)
		bool bDashing{};

	/** Float to see the current energy*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		float CurrentEnergyAmount{};

	/** Float to see the max energy*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		float MaxEnergyAmount = 90.f;

	/** Float to see the how much energy one dash takes up*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		float OneDashAmount{};

	/** Int to see the how much dashes a player has*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		int AmountOfDashes{2};

	/** Float to see how much energy a player replenishes per frame*/
	UPROPERTY(BlueprintReadOnly, Category = Dash)
		float EnergyReplenish{ 5.f };

	UPROPERTY(BluePrintReadOnly, Category = Energy)
		float TimeFreeDash{};

	UPROPERTY(BluePrintReadOnly, Category = Energy)
		float MaxTimeFreeDash = 2.f;

	UPROPERTY(BluePrintReadOnly, Category = Energy)
		bool bFreeDash = false;


	/** Reference to the primary weapon*/
	UPROPERTY(BlueprintReadWrite)
		ABaseWeapon* PrimaryWeapon {};

	/** Reference to the secondary weapon*/
	UPROPERTY(BlueprintReadWrite)
		ABaseWeapon* SecondaryWeapon {};

	UPROPERTY(BlueprintReadWrite)
		AFWBaseGameMode* GameMode {};


public:

	void AimUpInput(float value) { AimUp = value; }

	void AimRightInput(float value) { AimRight = value; }

	inline ABaseWeapon* GetPrimaryWeapon() const { return PrimaryWeapon; }
	inline ABaseWeapon* GetSecondaryWeapon() const { return SecondaryWeapon; }

private:

	float AimUp{};

	float AimRight{};

};
