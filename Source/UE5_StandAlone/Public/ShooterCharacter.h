// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class UE5_STANDALONE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void MoveUp(float Value);
	
	void TurnAtRate(float Value);
	
	void LookUpAtRate(float Value);

	void OnStartJump();
	
	void OnStopJump();
	
	void OnStopRunning();
	
	void OnStartRunning();

	void OnStartZoomIn();

	void OnStopZoomIn();
	
	void SetZoomIn(bool bNewState);
	
public:
	AShooterCharacter();
	
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsZoomIn() const;

	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) const;

protected:
	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
	float BaseLookUpRate;

	UPROPERTY()
	uint8 bIsZoomIn: 1;

private:
	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
	
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
