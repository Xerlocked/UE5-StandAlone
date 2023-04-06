// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAPickupActor.h"
#include "SAWeaponPickupActor.generated.h"

/**
 * 
 */
UCLASS()
class UE5_STANDALONE_API ASAWeaponPickupActor : public ASAUsableActor
{
	GENERATED_BODY()

	ASAWeaponPickupActor();
	
public:

	/* Class to add to inventory when picked up */
	UPROPERTY(EditDefaultsOnly, Category = "WeaponClass")
	TSubclassOf<class ABaseWeapon> WeaponClass;

	virtual void OnUsed(APawn* InstigatorPawn) override;
};
