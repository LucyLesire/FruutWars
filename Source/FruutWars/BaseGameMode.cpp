// Copyright Epic Games, Inc. All Rights Reserved.


#include "BaseGameMode.h"
#include "CharacterController.h"
#include "FWPlayerState.h"
#include "FWGameInstance.h"
#include "FWPlayerState.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "Kismet/GameplayStatics.h"

AFWBaseGameMode::AFWBaseGameMode()
{
	DefaultPawnClass = nullptr;

	PlayerControllerClass = ACharacterController::StaticClass();

	PlayerStateClass = AFWPlayerState::StaticClass();

	bUseSeamlessTravel = true;

	// Make an empty list of 4 player controllers that will be the ones playing the game
	ActivePlayers.Append({ {},{},{},{} });
	PlayersDeathState = { false,false,false,false };
}

void AFWBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	CurrentTime = MaxTimeBetweenSink + MaxTimeOfShake;
	TilePosDirty = true;
	//GetTile();

	for (int i{}; i < 8; ++i)
	{
		UGameplayStatics::CreatePlayer(this, i);
	}

	auto controllerIt = GetWorld()->GetPlayerControllerIterator();
	while (controllerIt)
	{
		if (auto playerState = controllerIt->Get()->GetPlayerState<AFWPlayerState>())
		{
			if (playerState->GetGamePlayerIndex() != -1)
			{
				ActivePlayers[playerState->GetGamePlayerIndex()] = Cast<ACharacterController>(controllerIt->Get());
			}
		}
		++controllerIt;
	}
}

float AFWBaseGameMode::GetTotalWeight() const
{
	float totalWeight{};

	for (auto tileWeight : TileWeights)
	{
		totalWeight += tileWeight.Value;
	}

	return totalWeight;
}

void AFWBaseGameMode::GetTile()
{
	float totalWeights = GetTotalWeight();
	float randomWeight = FMath::FRandRange(0, totalWeights);

	for (auto TileWeight : TileWeights)
	{
		if (randomWeight > TileWeight.Value)
			randomWeight -= TileWeight.Value;
		else
		{
			CurrentTile = TileWeight.Key;
			defaultPos = CurrentTile->GetActorLocation();
			StartTileShake(CurrentTile);
			return;
		}
	}
}

void AFWBaseGameMode::SinkTile(float dt)
{
	if (!bSinkTiles)
		return;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attempting to sink a tile"));

	//Don't sink if u don't have a tile
	if (!CurrentTile)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Tiles Selected to sink"));
		return;
	}
	
	CurrentTime -= dt;
	if(CurrentTime <= MaxTimeOfShake && CurrentTime > 0)
	{
		Shake();
	}
	else if(CurrentTime <= 0)
	{
		//Check if tile needs to be reset after shaking
		if (TilePosDirty)
		{
			//Reset tile after shaking
			CurrentTile->SetActorLocation(defaultPos);
			TilePosDirty = false;
			return;
		}

		//Get position and move downwards (framerate independent)
		auto oldPos = CurrentTile->GetActorLocation();
		oldPos.Z -= DropRate * dt;

		//Check if tile has dropped far enough
		if(oldPos.Z < MaxDropRate)
		{
			//Clamp so it doesn't go further then max drop
			oldPos.Z = FMath::Clamp(oldPos.Z, MaxDropRate, 0.f);
			CurrentTile->SetActorLocation(oldPos);

			//Reset time
			CurrentTime = MaxTimeBetweenSink;
			TileWeights.Remove(CurrentTile);

			//Reset position dirty
			TilePosDirty = true;

			StopTileShake(CurrentTile);

			GetTile();
		}
		else
		{
			//Move downwards
			CurrentTile->SetActorLocation(oldPos);
		}
	}
}

void AFWBaseGameMode::OnPlayerDeath(AActor* pActor)
{

	if (ABaseCharacter* character = Cast<ABaseCharacter>(pActor))
	{
		OnPlayerDied();

		const FTimespan maxDifference{ 0,0,20 };

		auto currentTime = FDateTime::UtcNow();

		TMap<AController*, float> controllerDamages{};

		auto& damageSources = character->HealthComponent->DamageSources;
		for (auto damageSource : damageSources)
		{
			// add the damage to a map
			auto timeDifference = currentTime - damageSource.timeStampDamage;
			if (timeDifference < maxDifference)
			{
				if (!controllerDamages.Contains(damageSource.damageActor))
					controllerDamages.Add(damageSource.damageActor);

				*controllerDamages.Find(damageSource.damageActor) += damageSource.damageAmount;
			}
		}

		// increase the damage amount of the last player who did damage
		if (damageSources.Num() > 0) {

			const float DamageMultiplier{ 2 };

			const auto& damageSource = damageSources.Last();

			if (!controllerDamages.Contains(damageSource.damageActor))
				controllerDamages.Add(damageSource.damageActor);

			*controllerDamages.Find(damageSource.damageActor) += damageSource.damageAmount * DamageMultiplier;
		}

		AController* mostDamagingPlayer{};
		float damage{};
		for (auto& controller : controllerDamages)
		{
			if (controller.Value > damage)
			{
				damage = controller.Value;
				mostDamagingPlayer = controller.Key;
			}
		}

		if (mostDamagingPlayer && !bStopScoring)
			if (auto playerState{Cast<AFWPlayerState>(mostDamagingPlayer->GetPlayerState<AFWPlayerState>())})
			{
				playerState->IncreaseScore(1);
			}

		if (APlayerController* controller = Cast<APlayerController>(character->GetController()))
		{
			PlayerDied(controller);
		}
	}
}

void AFWBaseGameMode::Shake()
{
	float sinOffset{};
	float cosOffset{};

	//Get sin and cos values for time and speed
	FMath::SinCos(&sinOffset, &cosOffset, CurrentTime * TileShakeSpeed);

	//Apply shake to X & Y
	CurrentTile->SetActorLocation(FVector{ defaultPos.X + (sinOffset * MaxMovementDisplacement), defaultPos.Y + (cosOffset * MaxMovementDisplacement), defaultPos.Z});
}

void AFWBaseGameMode::SpawnPlayers(TArray<FVector> locations) const
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attempting to spawn players"));

	// get the player controllers that are active from the game instance
	for (auto controller : ActivePlayers)
	{
		// check if they are not nullptr
		if (!IsValid(controller)) continue;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("	Valid controller found"));

		// Remove the pawn that they may be controlling
		if (auto Pawn{ controller->GetPawn() })
		{
			controller->UnPossess();
			Pawn->Destroy();
		}

		// Get the player state that contains the selected character class
		if (auto playerState = controller->GetPlayerState<AFWPlayerState>())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("		Valid player state"));

			// Get the selected char class and als check if it is valid
			if (auto selectedChar{ playerState->GetSelectedCharacter() })
			{
				// get a random position from the spawn list
				int randIndx = FMath::RandRange(0, locations.Num() - 1);

				FVector randomPos = locations[randIndx];
				locations.RemoveAtSwap(randIndx);

				// spawn the actor at one of those positions
				FActorSpawnParameters spawnParams;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto Pawn = GetWorld()->SpawnActor<ABaseCharacter>(selectedChar, randomPos, {}, spawnParams);

				if (Pawn)
				{
					Pawn->HealthComponent->OnDeathActor.AddDynamic(this, &AFWBaseGameMode::OnPlayerDeath);

					// make the controller posses the pawn
					controller->Possess(Pawn);
				}
			}
		}
	}
}

void AFWBaseGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSinkTiles)
		SinkTile(DeltaTime);
}

bool AFWBaseGameMode::JoinGame(APlayerController* controller)
{
	if (!controller) return false;

	auto playerState{ controller->GetPlayerState<AFWPlayerState>() };
	if (!playerState) return false;

	// if the player state was already registered, join the game instantly
	if (playerState->GetGamePlayerIndex() != -1)
	{
		ActivePlayers[playerState->GetGamePlayerIndex()] = controller;
		return true;
	}

	// if the player state is not registered, attempt to register him if there are enough spots
	for (int Idx{}; Idx < 4; ++Idx)
	{
		if (!ActivePlayers[Idx])
		{
			ActivePlayers[Idx] = controller;
			playerState->SetGamePlayerIndex(Idx);
			return true;
		}
	}
	return false;
}

void AFWBaseGameMode::LeaveGame(APlayerController* controller)
{
	auto playerState{ controller->GetPlayerState<AFWPlayerState>() };
	if (!playerState) return;

	if (playerState->GetGamePlayerIndex() != -1)
	{
		ActivePlayers[playerState->GetGamePlayerIndex()] = nullptr;
		playerState->SetGamePlayerIndex(-1);
	}
}

bool AFWBaseGameMode::JoinGamePosition(APlayerController* controller, int position)
{
	auto playerState{ controller->GetPlayerState<AFWPlayerState>() };
	if (!playerState) return false;

	if (!ActivePlayers[position])
	{
		ActivePlayers[position] = controller;
		playerState->SetGamePlayerIndex(position);
		return true;
	}
	return false;
}

void AFWBaseGameMode::SeamlessTravel(const FString& FURL)
{
	GetWorld()->ServerTravel(FURL, true);
}

void AFWBaseGameMode::EnableAllPlayerInput()
{
	for (auto player : ActivePlayers)
		if (IsValid(player))
			if (auto characterController{ Cast<ACharacterController>(player) })
				characterController->EnablePlayerInput();
}

void AFWBaseGameMode::DisableAllPlayerInput()
{
	for (auto player : ActivePlayers)
		if (IsValid(player))
			if (auto characterController{ Cast<ACharacterController>(player) })
				characterController->DisablePlayerInput();
}

void AFWBaseGameMode::PlayerDied(APlayerController* controller)
{
	if (!controller || bStopScoring) return;

	for (int i{}; i < 4; ++i)
	{
		if (ActivePlayers[i] == controller)
		{
			if (PlayersDeathState[i]) // if player is already dead
				return;
			PlayersDeathState[i] = true;
		}
	}

	int playersAlive{};
	APlayerController* alivePlayer{}; // keep a copy of an alive player in case there is only one alive
	for (int i{}; i < 4; ++i)
	{
		if (ActivePlayers[i] && !PlayersDeathState[i])
		{
			alivePlayer = ActivePlayers[i];
			++playersAlive;
		}
	}

	if (playersAlive <= 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("increased score"));
		if (playersAlive == 1)
			alivePlayer->GetPlayerState<AFWPlayerState>()->IncreaseScore(1);

		bStopScoring = true;
		RestartCurrentRound();
	}
}

TArray<APlayerController*> AFWBaseGameMode::GetPlayersByScore()
{
	auto sortedPlayers = TArray<APlayerController*>(ActivePlayers);

	sortedPlayers.RemoveAll([](APlayerController* player) {return !IsValid(player); });
	sortedPlayers.Sort([](APlayerController& p0, APlayerController& p1)
		{
			auto s0 = p0.GetPlayerState<AFWPlayerState>();
			auto s1 = p1.GetPlayerState<AFWPlayerState>();
			if (!s0 || !s1) return p0 < p1;
			return s0->GetRoundScore() > s1->GetRoundScore();
		});

	return sortedPlayers;
}

ABaseCharacter* AFWBaseGameMode::GetClosestPlayer(const FVector& position, float range, APlayerController* playerToIgnore)
{
	float closesDistanceSqr{INFINITY};
	ABaseCharacter* closestPlayer{};

	for (auto player : ActivePlayers)
	{
		if (player && player != playerToIgnore)
		{
			auto location = player->GetPawn()->GetActorLocation();
			float distance = FVector::DistSquared(position, location);
			if (distance < closesDistanceSqr)
			{
				if (ABaseCharacter* closestCharacter{ Cast<ABaseCharacter>(player->GetCharacter()) })
				{
					if (closestCharacter->HealthComponent->GetHealth() > 0.f)
					{
						closestPlayer = closestCharacter;
						closesDistanceSqr = distance;
					}
				}
			}
		}
	}

	return closestPlayer;
}

ABaseCharacter* AFWBaseGameMode::GetPlayerInDirection(const FVector& position, FVector direction, float anglePercent, APlayerController* playerToIgnore)
{
	float closesDistanceSqr{ INFINITY };
	direction = direction.GetSafeNormal();
	ABaseCharacter* closestPlayer{};

	for (auto player : ActivePlayers)
	{
		if (player && player != playerToIgnore)
		{
			auto location = player->GetPawn()->GetActorLocation();
			auto playerDirection = location - position;
			playerDirection.Normalize();
			float angle = FVector::DotProduct(direction, playerDirection);
			float distance = FVector::DistSquared(position, location);

			if (angle >= 1 - anglePercent && distance < closesDistanceSqr)
			{
				if (ABaseCharacter * closestCharacter{ Cast<ABaseCharacter>(player->GetPawn()) })
				{
					if (closestCharacter->HealthComponent->GetHealth() > 0.f)
					{
						closestPlayer = closestCharacter;
						closesDistanceSqr = distance;
					}
				}
			}
		}
	}

	return closestPlayer;
}

FVector AFWBaseGameMode::GetCenterOfPlayers()
{
	if (bStopScoring)
		return FVector{};

	FVector posSum{};
	posSum.X = 0;
	posSum.Y = 0;
	posSum.Z = 0;
	int players{};

	for (auto player : ActivePlayers)
	{
		if (player && player->GetPawn())
		{
			if (auto healthComp{ player->GetPawn()->GetComponentByClass(UHealthComponent::StaticClass()) })
			{
				if (Cast<UHealthComponent>(healthComp)->GetHealth() > 0.f)
				{
					auto location = player->GetPawn()->GetActorLocation();
					posSum.X += location.X;
					posSum.Y += location.Y;
					posSum.Z += location.Z;
					++players;
				}
			}
		}
	}

	return posSum / ((players != 0) ? players : 1);
}

float AFWBaseGameMode::GetFurthestDistanceFromPlayers(const FVector& location)
{
	if (bStopScoring)
		return 5000.f;

	float furtestDistance{};
	for (auto player : ActivePlayers)
	{
		if (player && player->GetPawn())
		{
			auto Playerlocation = player->GetPawn()->GetActorLocation();
			float distance{ FVector::Distance(Playerlocation, location) };
			furtestDistance = (distance > furtestDistance) ? distance : furtestDistance;
		}
	}

	return furtestDistance;
}

void AFWBaseGameMode::RestartCurrentRound()
{
	PlayersDeathState = { false,false,false,false };

	RestartRound();
}