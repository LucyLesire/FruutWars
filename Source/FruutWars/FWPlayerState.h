// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "FWPlayerState.generated.h"

class ABaseCharacter;
class UMaterialInstance;

/**
 * 
 */
UCLASS(BlueprintType)
class FRUUTWARS_API AFWPlayerState : public APlayerState
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<ABaseCharacter> SelectedCharacter{};

	UPROPERTY(BlueprintReadWrite)
		UMaterialInstance* SelectedSkinHead{};

	UPROPERTY(BlueprintReadWrite)
		UMaterialInstance* SelectedSkinBody{};

	UPROPERTY(BlueprintReadOnly)
		int GamePlayerIndex{ -1 };

	UPROPERTY(BlueprintReadOnly)
	int FWRoundScore{};

	UPROPERTY(BlueprintReadWrite)
		bool IsUsingKeyboard{};

	UPROPERTY(BlueprintReadWrite)
		bool IsReady{};

	UPROPERTY(BlueprintReadWrite)
		UTexture2D* PlayerDisplay;

	UPROPERTY(BlueprintReadWrite)
		UTexture2D* PlayerHUDDisplay;

	UPROPERTY(BlueprintReadWrite)
		FLinearColor PlayerColor;

public:

	inline int GetGamePlayerIndex() const { return GamePlayerIndex; }
	inline void SetGamePlayerIndex(int index) { GamePlayerIndex = index; }

	virtual void CopyProperties(APlayerState* PlayerState) override;

	inline TSubclassOf<ABaseCharacter> GetSelectedCharacter() const { return SelectedCharacter; }

	inline bool GetIsUsingKeyboard() const { return IsUsingKeyboard; }

	UFUNCTION()
		void IncreaseScore(int amount);

	UFUNCTION(BlueprintCallable)
		void ResetScore() { FWRoundScore = 0; }

	UFUNCTION(BlueprintCallable)
		int GetRoundScore() const { return FWRoundScore; }

	UFUNCTION(BlueprintCallable)
		UTexture2D* GetPlayerHudDisplay() const { return PlayerHUDDisplay; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreSignature, int, scoreAmount);
	UPROPERTY(BlueprintAssignable)
		FOnScoreSignature OnScoreIncrease;
};
