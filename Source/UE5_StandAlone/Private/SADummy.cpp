// Fill out your copyright notice in the Description page of Project Settings.


#include "SADummy.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values
ASADummy::ASADummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DummyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DummyMesh"));
	DummyMesh->SetupAttachment(RootComponent);

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	bReplicates = true;
}

void ASADummy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASADummy, CurrentHealth);
}

void ASADummy::SetCurrentHealth(float HealthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(HealthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ASADummy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageAmount;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

// Called when the game starts or when spawned
void ASADummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASADummy::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ASADummy::OnHealthUpdate()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
	else
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, healthMessage);
	}

	if(CurrentHealth <= 0.f)
	{
		DummyMesh->SetSimulatePhysics(true);
	}
}

// Called every frame
void ASADummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

