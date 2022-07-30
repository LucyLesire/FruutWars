// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


USTRUCT(BlueprintType)
struct FDamageSource
{
	GENERATED_BODY()

	UPROPERTY()
	float damageAmount;

	UPROPERTY()
	AController* damageActor;

	UPROPERTY()
	FDateTime timeStampDamage;

	FDamageSource()
	{
		damageAmount = 0;
		damageActor = nullptr;
		timeStampDamage = FDateTime();
	}

	FDamageSource(float damage, AController* source)
	{
		damageAmount = damage;
		damageActor = source;
		timeStampDamage = FDateTime::UtcNow();
	}
};

UCLASS(BlueprintType)
class FRUUTWARS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield")
	float MaxShield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield")
	float ShieldDepletionRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Invurnability")
	float MaxInvurnabilityTime;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TArray<FDamageSource> DamageSources;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category="Shield")
	float CurrentShield;

	UPROPERTY(BlueprintReadOnly, Category = "Invurnability")
	bool Invurnability;

	UPROPERTY(BlueprintReadOnly, Category = "Invurnability")
	float InvurnabilityTime;

	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetShieldPercentage() const;


	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void DoDamage(float amount, AController* source = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void DoKill();

	UFUNCTION(BlueprintCallable, Category = "Health")
	void DoHeal(float amount);


	UFUNCTION(BlueprintCallable, Category = "Shield")
	void AddShield(float amount);

	UFUNCTION(BlueprintCallable, Category = "Invurnability")
	void SetInvurnability(bool enable);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignatureActor, AActor*, pActor);
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignatureActor OnDeathActor;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageSignature, float, DamageAmount, AController*, Source);
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDamageSignature OnDamage;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealSignature, float, HealAmount);
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealSignature OnHeal;

	inline float GetHealth() const { return CurrentHealth; }
};
