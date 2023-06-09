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
	AShooterCharacter();
	
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
	
	void SetIsJumping(bool bNewState);

	UFUNCTION(BlueprintCallable)
	void SetSprinting(bool bNewState);
	
	void SetZoomIn(bool bNewState);

	void SetArmTransform(float DeltaTime);
	
	bool IsAlive() const;

	bool CanFire() const;
	
	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
    bool IsZoomIn() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Movement")
	bool IsFiring() const;
	
	void AddWeapon(class ABaseWeapon* NewWeapon);

	void EquipWeapon(class ABaseWeapon* NewWeapon);

	void SetCurrentWeapon(class ABaseWeapon* NewWeapon, class ABaseWeapon* LastWeapon = nullptr);

	EWeaponName GetCurrentWeaponName() const;
	
	UCameraComponent* GetPlayerFPSCamera() const;

	class ASAUsableActor* GetUsableInView() const;

	UPROPERTY(EditDefaultsOnly, Category = "Game|Weapon")
	ABaseWeapon* CurrentWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Weapon")
	FVector MuzzleOffset;
	
	UFUNCTION(BlueprintCallable, Category = Mesh)
	bool IsFirstPerson() const;

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

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
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

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FTransform DefaultArmTransform;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FTransform AimArmTransform;

private:
	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FPSCamera;

	// Current health of the Pawn
	UPROPERTY(EditAnywhere, Replicated, Category = Character)
	float Health;
	
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float SprintSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float ZoomSpeedModifier;

	UPROPERTY(EditDefaultsOnly, Category = Sockets)
	FName WeaponAttachPoint;

	float TimeElapsed;
	
	bool bWantsToFire;
public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) override;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
