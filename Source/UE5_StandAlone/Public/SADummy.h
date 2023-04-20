// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SADummy.generated.h"

UCLASS()
class UE5_STANDALONE_API ASADummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASADummy();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(BlueprintPure, Category=Health)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category=Health)
	FORCEINLINE float  GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category=Health)
	void SetCurrentHealth(float HealthValue);

	UFUNCTION(BlueprintCallable, Category=Health)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* DummyMesh;
	
	UPROPERTY(EditDefaultsOnly, Category=Health)
	float MaxHealth;

	UPROPERTY(ReplicatedUsing= OnRep_CurrentHealth)
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void OnHealthUpdate();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
