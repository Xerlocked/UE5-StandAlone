// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAUsableActor.h"
#include "SAPickupActor.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UE5_STANDALONE_API ASAPickupActor : public ASAUsableActor
{
	GENERATED_BODY()

	void BeginPlay() override;
	
	UFUNCTION()
	void OnRep_IsActive();
	
protected:

	ASAPickupActor();

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;
	
	virtual void RespawnPickup();

	virtual void OnPickedUp();

	virtual void OnRespawned();

public:

	virtual void OnUsed(APawn* InstigatorPawn) override;

	/* Immediately spawn on begin play */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	bool bStartActive;

	/* Will this item ever respawn */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	bool bAllowRespawn;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float RespawnDelay;

	/* Extra delay randomly applied on the respawn timer */
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float RespawnDelayRange;
};
