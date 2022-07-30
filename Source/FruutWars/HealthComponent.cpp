// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	MaxHealth = 100.f;
	MaxInvurnabilityTime = 5.f;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	InvurnabilityTime = MaxInvurnabilityTime;
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Reset invurnability
	if (Invurnability)
	{
		InvurnabilityTime -= DeltaTime;
		if (InvurnabilityTime <= 0.f)
		{
			InvurnabilityTime = MaxInvurnabilityTime;
			Invurnability = false;
		}
		return;
	}

	//Check if there is shield
	if(CurrentShield > 0.f)
	{
		//Deplete shield every frame (1 shield/second)
		CurrentShield -= ShieldDepletionRate * DeltaTime;
		CurrentShield = FMath::Clamp(CurrentShield, 0.f, MaxShield);
	}


}

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
}

float UHealthComponent::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float UHealthComponent::GetShieldPercentage() const
{
	return CurrentShield/MaxShield;
}

bool UHealthComponent::IsDead() const
{
	if(CurrentHealth <= 0.f)
	{
		return true;
	}
	return false;
}

void UHealthComponent::DoDamage(float amount, AController* source)
{
	//Check if Invurnable => no damage
	if (Invurnability)
		return;

	//Can't damage positive amounts
	if (amount < 0.f)
		return;

	const float orignalHealth = CurrentHealth;

	//Check for shield => use this over health
	if (CurrentShield > 0.f)
	{
		CurrentShield -= amount;
		if (CurrentShield < 0.f)
		{
			//Store the damage amount incase of overdamage
			const float healthDamageAmount = -CurrentShield;
			CurrentShield = 0.f;

			//Substract any damage left from the current health
			CurrentHealth -= healthDamageAmount;
			CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
		}
	}
	else
	{
		CurrentHealth -= amount;
		CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	}

	if (source) { // if there is a given source, add it to the damage sources
		FDamageSource damageSource;
		damageSource.damageAmount = amount;
		//damageSource.damageActor = Cast<APawn>(source)->GetController();
		damageSource.damageActor = source;
		damageSource.timeStampDamage = FDateTime::UtcNow();

		DamageSources.Add(damageSource);
	}

	OnDamage.Broadcast(amount, source);

	if (IsDead() && orignalHealth > CurrentHealth)
	{
		OnDeath.Broadcast();
		OnDeathActor.Broadcast(GetOwner());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Something Died");
	}
}

void UHealthComponent::DoKill()
{
	CurrentShield = 0;
	CurrentHealth = 0;

	OnDamage.Broadcast(10000, nullptr);

	OnDeath.Broadcast();
	OnDeathActor.Broadcast(GetOwner());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Something Died");
}


void UHealthComponent::AddShield(float amount)
{
	CurrentShield += amount;
	CurrentShield = FMath::Clamp(CurrentShield, 0.f, MaxShield);
}

void UHealthComponent::SetInvurnability(bool enable)
{
	Invurnability = enable;
}


void UHealthComponent::DoHeal(float amount)
{
	//Can't do negative heal
	if (amount < 0.f)
		return;

	CurrentHealth += amount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

	OnHeal.Broadcast(amount);
}
