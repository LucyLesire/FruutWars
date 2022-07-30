// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FWGameInstance.generated.h"

class ACharacterController;
class ABaseCharacter;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FRUUTWARS_API UFWGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	virtual void Init() override;

public:

	

};
