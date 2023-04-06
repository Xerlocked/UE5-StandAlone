// Fill out your copyright notice in the Description page of Project Settings.


#include "SAWeaponPickupActor.h"
#include "BaseWeapon.h"
#include "ShooterCharacter.h"

ASAWeaponPickupActor::ASAWeaponPickupActor()
{
	
}

void ASAWeaponPickupActor::OnUsed(APawn* InstigatorPawn)
{
	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(InstigatorPawn);
	if (MyPawn)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, SpawnInfo);

		MyPawn->AddWeapon(NewWeapon);

		Super::OnUsed(InstigatorPawn);
	}
}
