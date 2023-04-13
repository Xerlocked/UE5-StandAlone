// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

enum class EWeaponName : uint8;
class UCameraComponent;

UCLASS()
class UE5_STANDALONE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void MoveUp(float Value);
	
	void TurnAtRate(float Value);
	
	void LookUpAtRate(float Value);

	void Use();

	void OnJump();

	void OnStopRunning();
	
	void OnStartRunning();

	void OnStartZoomIn();

	void OnStopZoomIn();

	void OnFireStart();

	void OnFireStop();

	void StartWeaponFire();

	void StopWeaponFire();
	
	void SetIsJumping(bool NewState);

	void SetSprinting(bool NewState);
	
	void SetZoomIn(bool bNewState);

	/** check if pawn is still alive */
	bool IsAlive() const;

	bool CanFire() const;
	
	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
    bool IsZoomIn() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
	bool IsFiring() const;
	
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	void Fire();

	void AddWeapon(class ABaseWeapon* NewWeapon);

	void EquipWeapon(class ABaseWeapon* NewWeapon);

	void SetCurrentWeapon(class ABaseWeapon* NewWeapon, class ABaseWeapon* LastWeapon = nullptr);

	EWeaponName GetCurrentWeaponName() const;

	class ASAUsableActor* GetUsableInView() const;
	
public:
	AShooterCharacter();

	// Current health of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Weapon")
	class ABaseWeapon* CurrentWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Weapon")
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Projectile")
	TSubclassOf<class ABaseProjectile> ProjectileClass;
	
	UFUNCTION(BlueprintCallable, Category = Mesh)
	virtual bool IsFirstPerson() const;

	FName GetWeaponAttachPoint() const;

	USkeletalMeshComponent* GetSpecificPawnMesh(bool WantFirstPerson) const;

	USkeletalMeshComponent* GetPawnMesh() const;

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	FRotator GetAimOffsets() const;

	float GetSprintSpeedModifier() const;
	
	float GetZoomSpeedModifier() const;
	
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) const;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

protected:
	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate;

	/** Base lookup rate, in deg/sec. Other scaling may affect final lookup rate. */
	float BaseLookUpRate;

	UPROPERTY()
	uint8 bIsZoomIn: 1;

	UPROPERTY()
	bool bIsJumping;

	UPROPERTY()
	bool bWantsToRun;

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FPSCamera;
	
	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float SprintSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float ZoomSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = Sockets)
	FName WeaponAttachPoint;

	bool bWantsToFire;
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
