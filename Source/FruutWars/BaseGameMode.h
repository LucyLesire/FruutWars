// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

class ACharacterController;
class ASpawnPoint;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API AFWBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFWBaseGameMode();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		bool JoinGame(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		void LeaveGame(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		bool JoinGamePosition(APlayerController* controller, int position);

	/**
	 * Returns an array of size 4 of the active player.
	 * Some may be Nullptr
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
		const TArray<APlayerController*>& GetActivePlayers() const { return ActivePlayers; }

	UFUNCTION(BlueprintCallable)
		void SeamlessTravel(const FString& FURL);

	UFUNCTION(BlueprintCallable)
		void EnableAllPlayerInput();

	UFUNCTION(BlueprintCallable)
		void DisableAllPlayerInput();

	/**
	 * Call this functions whenever a player has died
	 * It will restart the round if there is only 1 player left
	 */
	UFUNCTION(BlueprintCallable)
		void PlayerDied(APlayerController* controller);

	UFUNCTION(BlueprintCallable)
		TArray<APlayerController*> GetPlayersByScore();

	UFUNCTION(BlueprintCallable)
		ABaseCharacter* GetClosestPlayer(const FVector& position, float range, APlayerController* playerToIgnore = nullptr);

	UFUNCTION(BlueprintCallable)
		ABaseCharacter* GetPlayerInDirection(const FVector& position, FVector direction, float anglePercent = 0.1f, APlayerController* playerToIgnore = nullptr);

	UFUNCTION(BlueprintCallable)
		FVector GetCenterOfPlayers();

	UFUNCTION(BlueprintCallable)
		float GetFurthestDistanceFromPlayers(const FVector& location);

protected:

	UPROPERTY(BlueprintReadOnly)
		TArray<APlayerController*> ActivePlayers;
	
	TArray<bool> PlayersDeathState;

	UFUNCTION()
		void RestartCurrentRound();

	UFUNCTION(BlueprintImplementableEvent)
		void RestartRound();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		TMap<AActor*, float> TileWeights{};

	float GetTotalWeight() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitializeTiles();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StartTileShake(AActor* tile);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StopTileShake(AActor* tile);

	UFUNCTION(BlueprintCallable)
		void GetTile();

	UFUNCTION(BlueprintCallable)
		void SinkTile(float dt);

	/**
	 * Function used to bind the the on actor death event of the health component
	 * Will also calculate the points and give it to the player
	 */ 
	UFUNCTION()
		void OnPlayerDeath(AActor* pActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnPlayerDied();

	UPROPERTY(BlueprintReadWrite)
		bool bSinkTiles{};

	UPROPERTY(BlueprintReadWrite)
		bool bStopScoring{};

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Tile Sinking")
		AActor* CurrentTile;

	UPROPERTY(BlueprintReadOnly, Category = "Tile Sinking")
		float CurrentTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float MaxTimeBetweenSink;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float MaxDropRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float DropRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float MaxMovementDisplacement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float MaxTimeOfShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Sinking")
		float TileShakeSpeed;
public:

	UFUNCTION(BlueprintCallable)
		void SpawnPlayers(TArray<FVector> locations) const;

	bool CanPlayersScore() { return !bStopScoring; }

private:
	void Shake();

	FVector defaultPos{};

	bool TilePosDirty{};

};
