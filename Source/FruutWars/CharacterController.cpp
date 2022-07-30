// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterController.h"
#include "BaseCharacter.h"
#include "BaseGameMode.h"
#include "FWGameInstance.h"
#include "FWPlayerState.h"
#include "HealthComponent.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"

ACharacterController::ACharacterController()
{
	
}

void ACharacterController::BeginPlay()
{
	Super::BeginPlay();
	bAutoManageActiveCameraTarget = false;

	if (auto playerState = GetPlayerState<AFWPlayerState>())
	{
		bIsUsingKeyboard = playerState->GetIsUsingKeyboard();
		if (bIsUsingKeyboard)
		{
			SetShowMouseCursor(true);
		}
	}
}

void ACharacterController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);

	ActorList.Add(GetPlayerState<AFWPlayerState>());

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("added player state to seamless travel"));

}


void ACharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PossesedCharacter = Cast<ABaseCharacter>(InPawn);
	PossesedCharacter->HealthComponent->OnDeath.AddDynamic(this, &ACharacterController::NotifyDeathGameMode);

	if (!PossesedCharacter)
	{
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Red, TEXT("Unable to cast to Base Character"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 5, FColor::Blue, TEXT("possesed a character"));
	}
}

void ACharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	PossesedCharacter = nullptr;
}

bool ACharacterController::JoinGame()
{
	if (auto gamemode{ Cast<AFWBaseGameMode>(GetWorld()->GetAuthGameMode())})
	{
		return gamemode->JoinGame(this);
	}
	return false;
}

void ACharacterController::LeaveGame()
{
	if (auto gamemode{ Cast<AFWBaseGameMode>(GetWorld()->GetAuthGameMode()) })
	{
		gamemode->LeaveGame(this);
	}
}

void ACharacterController::BeginDestroy()
{
	Super::BeginDestroy();

	//LeaveGame();
}

void ACharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bStunned)
	{
		StunDuration -= DeltaSeconds;
		if (StunDuration <= 0.f)
		{
			bStunned = false;
			EnablePlayerInput();
		}
	}

	if (bIsUsingKeyboard && PossesedCharacter)
	{
		FHitResult hit{};
		GetHitResultUnderCursor(ECollisionChannel::ECC_EngineTraceChannel2, false, hit);
		FVector Direction = hit.Location - PossesedCharacter->GetActorLocation();
		Direction.Z = 0.f;
		Direction.Normalize();
		PossesedCharacter->AimUpInput(Direction.X);
		PossesedCharacter->AimRightInput(Direction.Y);
	}
}

void ACharacterController::SetupInputComponent()
{
	APlayerController::SetupInputComponent();

	if (InputComponent)
	{
		//InputComponent->BindAction("BecomeActiveController", IE_Pressed, this, &ACharacterController::ActivateController);
		//InputComponent->BindAction("BecomeActiveKeyboard", IE_Pressed, this, &ACharacterController::ActivateKeyboard);

		InputComponent->BindAction("Dash", IE_Pressed, this, &ACharacterController::Dash);
		InputComponent->BindAction("Pauze", IE_Pressed, this, &ACharacterController::PauseGame);

		InputComponent->BindAxis("MoveUp", this, &ACharacterController::MoveUp);
		InputComponent->BindAxis("MoveRight", this, &ACharacterController::MoveRight);
		InputComponent->BindAxis("LookUp", this, &ACharacterController::LookUp);
		InputComponent->BindAxis("LookRight", this, &ACharacterController::LookRight);

		InputComponent->BindAxis("SecondaryFire", this, &ACharacterController::SecondaryFire);
		InputComponent->BindAxis("PrimaryFire", this, &ACharacterController::PrimaryFire);
	}
}

void ACharacterController::PrimaryFire(float value)
{
	if (abs(value) > 0.1f && PossesedCharacter && bPlayerInputEnabled)
	{
		PossesedCharacter->PrimaryFire();
	}
}

void ACharacterController::SecondaryFire(float value)
{
	if (abs(value) > 0.1f && PossesedCharacter && bPlayerInputEnabled)
	{
		PossesedCharacter->SecondaryFire();
	}
}

void ACharacterController::Dash() 
{
	if (PossesedCharacter && bPlayerInputEnabled)
	{
		PossesedCharacter->Dash();
	}
}

void ACharacterController::MoveUp(float value) 
{
	if (PossesedCharacter && bPlayerInputEnabled)
	{
		PossesedCharacter->AddMovementInput({ 1,0,0 }, value);
	}
}

void ACharacterController::MoveRight(float value) 
{
	if (PossesedCharacter && bPlayerInputEnabled)
	{
		PossesedCharacter->AddMovementInput({ 0,1,0 }, value);
	}
}

void ACharacterController::LookUp(float value) 
{
	if (PossesedCharacter)
	{
		PossesedCharacter->AimUpInput(value);
	}
}

void ACharacterController::LookRight(float value) 
{
	if (PossesedCharacter)
	{
		PossesedCharacter->AimRightInput(value);
	}
}

void ACharacterController::PauseGame() 
{
	//TODO bring up pause menu here

	Pause();
}

void ACharacterController::NotifyDeathGameMode()
{
	//if (auto gamemode = Cast<AFWBaseGameMode>(GetWorld()->GetAuthGameMode()))
	//{
	//	gamemode->PlayerDied(this);
	//}
}

void ACharacterController::Stun(float duration)
{
	if (!bStunned)
	{
		MaxStunDuration = duration;
		StunDuration = duration;
		
		DisablePlayerInput();
		bStunned = true;
	}
}