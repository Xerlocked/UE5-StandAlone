// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SAGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE5_STANDALONE_API ASAGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	ASAGameMode();
	
public:

	virtual bool CanDealDamage(class ASAPlayerState* DamageCauser, class ASAPlayerState* DamagedPlayer) const;
	
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

private:
};
