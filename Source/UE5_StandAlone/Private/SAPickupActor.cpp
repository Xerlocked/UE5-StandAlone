// Fill out your copyright notice in the Description page of Project Settings.


#include "SAPickupActor.h"
#include "Net/UnrealNetwork.h"

void ASAPickupActor::BeginPlay()
{
	Super::BeginPlay();

	RespawnPickup();
}

void ASAPickupActor::OnRep_IsActive()
{
	if (bIsActive)
	{
		OnRespawned();
	}
	else
	{
		OnPickedUp();
	}
}

ASAPickupActor::ASAPickupActor()
{
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	bIsActive = false;
	bStartActive = true;
	bAllowRespawn = true;
	RespawnDelay = 5.0f;
	RespawnDelayRange = 5.0f;

	SetReplicates(true);
}

void ASAPickupActor::RespawnPickup()
{
	bIsActive = true;
	OnRespawned();
}

void ASAPickupActor::OnPickedUp()
{
	if (MeshComp)
	{
		MeshComp->SetVisibility(false);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASAPickupActor::OnRespawned()
{
	bIsActive = true;
	OnRespawned();
}

void ASAPickupActor::OnUsed(APawn* InstigatorPawn)
{
	Super::OnUsed(InstigatorPawn);
}

void ASAPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASAPickupActor, bIsActive);
}
