// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

class ABaseCharacter;

/**
 * A Player Controller in charge of controlling the player during gameplay.
 * Is able to to stop functionality for the player while still using other functions like pausing the game
 */
UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

protected:

	ACharacterController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	/** Asks the game instance to join the game*/
	UFUNCTION(BlueprintCallable)
		bool JoinGame();

	/** Asks the game instance to leave the game*/
	UFUNCTION(BlueprintCallable)
		void LeaveGame();

	virtual void BeginDestroy() override;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;

protected:

	virtual void SetupInputComponent() override;

	void PrimaryFire(float value);

	void SecondaryFire(float value);

	void Dash();

	void MoveUp(float value);

	void MoveRight(float value);

	void LookUp(float value);

	void LookRight(float value);

	void PauseGame();

	UFUNCTION()
		void NotifyDeathGameMode();



public:

	UFUNCTION(BlueprintCallable)
		void EnablePlayerInput() { bPlayerInputEnabled = true; }

	UFUNCTION(BlueprintCallable)
		void DisablePlayerInput() { bPlayerInputEnabled = false; }

	UFUNCTION(BlueprintCallable)
		void Stun(float duration);

protected:

	/**
	 * VARIABLES
	 */

	/** The type of player that will spawn*/
	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<ABaseCharacter> PlayerClass;

	ABaseCharacter* PossesedCharacter{};

	/** If the player is using a keyboard and mouse*/
	UPROPERTY(BlueprintReadWrite)
	bool bIsUsingKeyboard{};

	bool bPlayerInputEnabled{};

	bool bIsActivePlayer{};

	UPROPERTY(BluePrintReadOnly, Category = Stun)
		bool bStunned = false;

	UPROPERTY(BluePrintReadOnly, Category = Stun)
		float StunDuration{};

	UPROPERTY(BlueprintReadWrite, Category = Stun)
		float MaxStunDuration{};
};

