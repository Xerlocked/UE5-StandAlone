// Fill out your copyright notice in the Description page of Project Settings.


#include "SAGameMode.h"

#include "SAGameState.h"
#include "SAPlayerState.h"
#include "SASpectatorPawn.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"

ASAGameMode::ASAGameMode()
{
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	PlayerStateClass = ASAPlayerState::StaticClass();
	GameStateClass = ASAGameState::StaticClass();
	SpectatorClass = ASASpectatorPawn::StaticClass();
}

bool ASAGameMode::CanDealDamage(ASAPlayerState* DamageCauser, ASAPlayerState* DamagedPlayer) const
{
	/* Allow damage to self */
	if (DamagedPlayer == DamageCauser)
	{
		return true;
	}
	
	return false;
}

float ASAGameMode::ModifyDamage(float Damage, AActor* DamagedActor, FDamageEvent const& DamageEvent,
                                AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	const AShooterCharacter* DamagedPawn = Cast<AShooterCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		ASAPlayerState* DamagedPlayerState = Cast<ASAPlayerState>(DamagedPawn->GetPlayerState());
		ASAPlayerState* InstigatorPlayerState = Cast<ASAPlayerState>(EventInstigator->PlayerState);

		// Check for friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.f;
		}
	}

	return ActualDamage;
}
