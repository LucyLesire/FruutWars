// Fill out your copyright notice in the Description page of Project Settings.


#include "FWPlayerState.h"

#include "BaseGameMode.h"
#include "CharacterController.h"
#include "GameFramework/GameModeBase.h"

void AFWPlayerState::BeginPlay()
{
	Super::BeginPlay();

	
}

void AFWPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (auto PS = Cast<AFWPlayerState>(PlayerState))
	{
		PS->GamePlayerIndex = GamePlayerIndex;
		PS->IsUsingKeyboard = IsUsingKeyboard;
		PS->SelectedCharacter = SelectedCharacter;
		PS->SelectedSkinHead = SelectedSkinHead;
		PS->SelectedSkinBody = SelectedSkinBody;
		PS->FWRoundScore = FWRoundScore;
		PS->PlayerDisplay = PlayerDisplay;
		PS->PlayerHUDDisplay = PlayerHUDDisplay;
		PS->PlayerColor = PlayerColor;
	}
}

void AFWPlayerState::IncreaseScore(int amount)
{
	FWRoundScore += amount;

	OnScoreIncrease.Broadcast(amount);
}

